//===- Hello.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include <llvm/Support/CommandLine.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/ToolOutputFile.h>

#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Argument.h>
#include <llvm/IR/User.h>
#include <llvm/IR/DebugLoc.h>

#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>


using namespace llvm;
static ManagedStatic<LLVMContext> GlobalContext;
static LLVMContext &getGlobalContext() { return *GlobalContext; }
/* In LLVM 5.0, when  -O0 passed to clang , the functions generated with clang will
 * have optnone attribute which would lead to some transform passes disabled, like mem2reg.
 */
struct EnableFunctionOptPass: public FunctionPass {
    static char ID;
    EnableFunctionOptPass():FunctionPass(ID){}
    bool runOnFunction(Function & F) override{
        if(F.hasFnAttribute(Attribute::OptimizeNone))
        {
            F.removeFnAttr(Attribute::OptimizeNone);
        }
        return true;
    }
};

char EnableFunctionOptPass::ID=0;

	
///!TODO TO BE COMPLETED BY YOU FOR ASSIGNMENT 2
///Updated 11/10/2017 by fargo: make all functions
///processed by mem2reg before this pass.
struct FuncPtrPass : public ModulePass {
  static char ID; // Pass identification, replacement for typeid
  FuncPtrPass() : ModulePass(ID) {}


  std::map<int, std::vector<std::string>> call_graph;
  std::vector<std::string> funcList;
  
  void Push_funcList(std::string funcname)
  {
    if (find(funcList.begin(), funcList.end(), funcname) == funcList.end())
    {
      funcList.push_back(funcname);
    }
  }
  void ShowResult()
  {
    for (std::map<int, std::vector<std::string>>::iterator iter = call_graph.begin(), e = call_graph.end(); iter != e; iter++){
      if (iter->second.size() == 0){continue;}
      errs() << iter->first << " : ";
      for (auto ii = iter->second.begin(), ie = iter->second.end() - 1; ii != ie; ii++){
        errs() << *ii << ", ";
      }
      errs() << *(iter->second.end() - 1) << "\n";
    }
  }
  
  bool runOnModule(Module &M) override {
    for (Module::iterator fi = M.begin(), fe = M.end(); fi != fe; fi++) // 函数列表iterator的类型定义，得到的fi是iterator定义可以继续用来定义,如果需要处理或者记录该类型的语句需要将一个iterator转换为一个类指针
    {   // Function *func_tmp = dyn_cast<Function>(fi); //相当于Function& func_tmp = *fi;
      for (Function::iterator bi = fi->begin(), be = fi->end(); bi != be; bi++)// for basicblock in function (function list)
      {
        for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++)// for instruction in basicblock
        {
          Instruction *inst = dyn_cast<Instruction>(ii);
          if (isa<CallInst>(inst) || isa<InvokeInst>(inst)) {
            CallInst *callInst = dyn_cast<CallInst>(inst);
            Function* func = callInst->getCalledFunction(); // 通过getCalledFunction()函数来获得所调用的函数
            if (func) {// func返回非null，则为直接调用
              std::string func_name = func->getName();
              if(!(func_name == std::string("llvm.dbg.value"))){
                funcList.clear();
                Push_funcList(func_name);
                int callline = callInst->getDebugLoc().getLine();
                if (call_graph.find(callline) == call_graph.end()){
                  call_graph.insert(std::pair<int, std::vector<std::string>>(callline, funcList));
                }else{
                  auto tmp = call_graph.find(callline);
                  tmp->second.push_back(func_name);
                }
              }
            }else{//间接调用
              funcList.clear();
              Value *value = callInst->getCalledValue(); // 获取这个value的节点，然后看他实际上是什么类型的过程变量再进行处理
              if (PHINode *phiNode = dyn_cast<PHINode>(value)){ //说明这个函数调用是个phi节点
                ParsePhiNode(phiNode);
              }else if (Argument *argument = dyn_cast<Argument>(value)){ //说明这个函数调用是通过参数传递进来的，需要进一步调用这个参数的来源
                ParseArgument(argument);
              }else if (CallInst *sub_callInst = dyn_cast<CallInst>(value)){ //调用给定的函数
                ParseCallInst(sub_callInst);
              }
              int callline = callInst->getDebugLoc().getLine();
              call_graph.insert(std::pair<int,std::vector<std::string>>(callline, funcList));
            }
          }
        }
      }
    }
    ShowResult();
    return false;
  }

  void ParseFunction_Ret(Function *function){
    for (Function::iterator bi = function->begin(), be = function->end(); bi != be; bi++)
    {
      for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++)
        {
          Instruction *inst = dyn_cast<Instruction>(ii);
          if(ReturnInst *RI = dyn_cast<ReturnInst>(inst)){
            Value *Rvalue = RI->getReturnValue();
            ParseInst(Rvalue);
          }
        }
    }
  }

  void ParseInst(Value *value){
    if (Argument *argument = dyn_cast<Argument>(value)){
      ParseArgument(argument);
    }else if (PHINode *pHINode = dyn_cast<PHINode>(value)){
      ParsePhiNode(pHINode);
    }else if (CallInst *callInst = dyn_cast<CallInst>(value)){
      ParseCallInst(callInst);
    }
  }

  void ParseValue(Value *value){
    if (PHINode *phiNode = dyn_cast<PHINode>(value)){
      ParsePhiNode(phiNode);
    }else if (Function *func = dyn_cast<Function>(value)){
      Push_funcList(func->getName());
    }else if (Argument *argument = dyn_cast<Argument>(value)){
      ParseArgument(argument);
    }
  }

  // Φ节点
  void ParsePhiNode(PHINode* phiNode){
    // incoming_values()的返回值类型是op_range，返回的是phi节点可能的定义
    for(Value* inComingVal : phiNode->incoming_values()){
      if (phiNode = dyn_cast<PHINode>(inComingVal)) {
        ParsePhiNode(phiNode);
      } else if (Argument* argument = dyn_cast<Argument>(inComingVal)) {
        ParseArgument(argument);
      } else if (Function* func = dyn_cast<Function>(inComingVal)) {
        Push_funcList(func->getName());
      }
    }
  }

  void ParseArgument(Argument* argument){
    // Return the index of this formal argument
    int index = argument->getArgNo();
    Function* parentFunc = argument->getParent();
    for (User* user : parentFunc->users()) {//get all user with the Function
      if (CallInst* callInst = dyn_cast<CallInst>(user)) {//判断使用这个operator/function/constant/instruction....的是个啥
        Value* value = callInst->getOperand(index);// 获得参数操作数
        if (callInst->getCalledFunction() != parentFunc){ // call函数与自己相等，需要去遍历那个函数再解析返回值
          Function* function = callInst->getCalledFunction();
          ParseFunction_Ret(function);
        }else{// call节点与自己相等，直接解析，自己递归
          ParseValue(value);  
        }
      }else if (PHINode *phiNode = dyn_cast<PHINode>(user)){ // 如果这个使用的过程变量不是一个callinst，而是一个phiNode
        for (User* user : phiNode->users()) {
          if (CallInst* callInst = dyn_cast<CallInst>(user)) {
            Value* value = callInst->getOperand(index);//拿到对应index的value
            ParseValue(value);
          }
        }
      }
    }
  }

  void ParseCallInst(CallInst *callinst){
    if(callinst->getCalledFunction()){// 返回调用的函数，如果这是间接函数调用，则返回null
      std::string func_name = callinst->getCalledFunction()->getName();
      ParseFunction_Ret(callinst->getCalledFunction());
    }else{
      Value *value = callinst->getCalledValue();
      if (PHINode *phiNode = dyn_cast<PHINode>(value)){
        for(Value* inComingVal : phiNode->incoming_values()){
          if(Function* func = dyn_cast<Function>(inComingVal)){
            ParseFunction_Ret(func);
          }
        }
      }else if (Argument *argument = dyn_cast<Argument>(value)){ //调用的是个函数
        int index = argument->getArgNo();
        Value* tmp_value = callinst->getArgOperand(index);
        argument = dyn_cast<Argument>(tmp_value);
        ParseArgument(argument);
      }
      else if (CallInst *callInst = dyn_cast<CallInst>(value)){ //调用给定的函数
        ParseCallInst(callInst);
      }
    }
  }
};


char FuncPtrPass::ID = 0;
static RegisterPass<FuncPtrPass> X("funcptrpass", "Print function call instruction");

static cl::opt<std::string>
InputFilename(cl::Positional,
              cl::desc("<filename>.bc"),
              cl::init(""));


int main(int argc, char **argv) {
   LLVMContext &Context = getGlobalContext(); //LLVMContext: 线程上下文，通个getGlobalContext获得
   SMDiagnostic Err;
   // Parse the command line to read the Inputfilename
   // ./llvmassignment --help
   cl::ParseCommandLineOptions(argc, argv,
                              "FuncPtrPass \n My first LLVM too which does not do much.\n");


   // Load the input module
   std::unique_ptr<Module> M = parseIRFile(InputFilename, Err, Context);//parseIRFile调用解析使用clang++ -S -emit-llvm test.cpp创建的IR文件， parseIRFile操作此IR，然后将IR写回文件。
   if (!M) {
      Err.print(argv[0], errs());
      return 1;
   }

   llvm::legacy::PassManager Passes; // 其实passmanager是一个队列。对于单个pass，核心部分在于实现其runOnModule函数
   	
   ///Remove functions' optnone attribute in LLVM5.0
   Passes.add(new EnableFunctionOptPass());
   ///Transform it to SSA
   Passes.add(llvm::createPromoteMemoryToRegisterPass());

   /// Your pass to print Function and Call Instructions
   Passes.add(new FuncPtrPass());
   Passes.run(*M.get());
}

