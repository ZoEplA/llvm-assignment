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
  int debug=0;
  
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
    // errs() << "Hello: ";
    // errs().write_escaped(M.getName()) << '\n';
    // M.dump();
    // errs()<<"------------------------------\n";
    // Function 迭代
    for (Module::iterator fi = M.begin(), fe = M.end(); fi != fe; fi++) // 函数列表iterator的类型定义，得到的fi是iterator定义可以继续用来定义,如果需要处理或者记录该类型的语句需要将一个iterator转换为一个类指针
    {
      // Function *func_tmp = dyn_cast<Function>(fi); //相当于
      if(debug == 1){
        Function& func_tmp = *fi; //and then could -->
        errs() << "[*] Function = " << func_tmp.getName() << '\n';
      }
      // for basicblock in function (function list)
      for (Function::iterator bi = fi->begin(), be = fi->end(); bi != be; bi++)
      {
        // for instruction in basicblock
        if(debug == 1){
          BasicBlock& bb_tmp = *bi; //and then could -->
          errs() << "[*] BasicBlock = " << bb_tmp.getName() << '\n';
        }
        for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++)
        {
          if(debug == 1){
            Instruction& inst_tmp = *ii; //and then could -->
            errs() << "[*] Instruction = " << inst_tmp.getName() << '\n';
          }
          Instruction *inst = dyn_cast<Instruction>(ii);
          if (isa<CallInst>(inst) || isa<InvokeInst>(inst)) {
            CallInst *callInst = dyn_cast<CallInst>(inst);
            Function* func = callInst->getCalledFunction(); // 通过getCalledFunction()函数来获得所调用的函数
            // func返回非null，则为直接调用
            if (func) {
              std::string func_name = func->getName();
              if(!(func_name == std::string("llvm.dbg.value"))){
                errs() << "[+] callline = " << callInst->getDebugLoc().getLine() << "\n";
                funcList.clear();
                errs() << "[+] Push_Function = " << func_name << "\n";
                Push_funcList(func_name);
                int callline = callInst->getDebugLoc().getLine();

                if (call_graph.find(callline) == call_graph.end()){
                  errs() << " Insert================\n";
                  call_graph.insert(std::pair<int, std::vector<std::string>>(callline, funcList));
                }else{
                  auto tmp = call_graph.find(callline);
                  tmp->second.push_back(func_name);
                }
              }
            }else{//间接调用
              errs() << "[+] callline = " << callInst->getDebugLoc().getLine() << "\n";
              funcList.clear();
              Value *value = callInst->getCalledValue(); // 获取这个value的节点，然后看他实际上是什么类型的过程变量再进行处理
              errs() << "[*] getCalledValue = " << value->getName() << '\n';

              if (PHINode *phiNode = dyn_cast<PHINode>(value))
              {
                //说明call的时候这里有个phinode
                errs() << "That is phiNode.\n";
                ParsePhiNode(phiNode);
              }
              else if (Argument *argument = dyn_cast<Argument>(value))//调用的是个函数
              {
                errs() << "That is argument.\n";
                ParseArgument(argument);
              }
              else if (CallInst *sub_callInst = dyn_cast<CallInst>(value)) //调用给定的函数
              {
                errs() << "That is callInst.\n";
                ParseCallInst(sub_callInst);
              }
              errs() << "insert=======================\n";
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
    errs() << "[*] ParseFunction_Ret = " << function->getName() << '\n';
    for (Function::iterator bi = function->begin(), be = function->end(); bi != be; bi++)
    {
      for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++)
        {
          Instruction *inst = dyn_cast<Instruction>(ii);
          if(ReturnInst *RI = dyn_cast<ReturnInst>(inst)){
            Value *Rvalue = RI->getReturnValue();
            errs() << "[*] ParseFunction_Ret = " << RI->getName() << '\n';
            ParseInst(Rvalue);
          }
        }
    }
  }

  void ParseInst(Value *value){
    if (Argument *argument = dyn_cast<Argument>(value)){
      errs() << "[*] ParseInst Argument.\n";
      ParseArgument(argument);
    }else if (PHINode *pHINode = dyn_cast<PHINode>(value)){
      errs() << "[*] ParseInst PHINode.\n";
      ParsePhiNode(pHINode);
    }else if (CallInst *callInst = dyn_cast<CallInst>(value)){
      errs() << "[*] ParseInst CallInst.\n";
      ParseCallInst(callInst);
    }
  }

  // Φ节点
  void ParsePhiNode(PHINode* phiNode){
    // incoming_values()的返回值类型是op_range
    for(Value* inComingVal : phiNode->incoming_values()){
      errs() << "[*] inComingVal = " << inComingVal->getName() << '\n';
      if (phiNode = dyn_cast<PHINode>(inComingVal)) {
        errs() << "[*] PHINode = " << phiNode->getName() << '\n';
        errs() << "That is PHINode.\n";
        ParsePhiNode(phiNode);
      } else if (Argument* argument = dyn_cast<Argument>(inComingVal)) {
        errs() << "[*] Argument = " << argument->getName() << '\n';
        errs() << "That is argument.\n";
        ParseArgument(argument);
      } else if (Function* func = dyn_cast<Function>(inComingVal)) {
        errs() << "[*] Function = " << func->getName() << '\n';
        errs() << "That is Function.\n";
        errs() << "[+] Push_Function = " << func->getName() << "\n";
        Push_funcList(func->getName());
      }
    }
  }

  void ParseArgument(Argument* argument){
    // Return the index of this formal argument
    int index = argument->getArgNo();
    errs() << "[*] callinst Argument index  = " << index << "\n";
    Function* parentFunc = argument->getParent();
    errs() << "[*] callinst Argument Function  = " << parentFunc->getName() << "\n";
    for (User* user : parentFunc->users()) {//get all user with the Function
      errs() << "[*] callinst Argument parentFunc->users  = " << user->getName() << "\n";
      // Value* value = user->getOperand(index);
      // errs() << "[*] ParseArgument  value = " << value->getName() << "\n"; // call
      if (CallInst* callInst = dyn_cast<CallInst>(user)) {//判断使用这个operator/function/constant/instruction....的是个啥
        Value* value = callInst->getOperand(index);// 获得参数操作数
        errs() << "[*] ParseArgument parentFunc  = " << parentFunc->getName() << "\n";
        errs() << "[*] ParseArgument callInst->getCalledFunction()  = " << callInst->getCalledFunction()->getName() << "\n";
        if (callInst->getCalledFunction() != parentFunc){ // call函数与自己相等，需要去遍历那个函数再解析返回值
          Function* function = callInst->getCalledFunction();
          errs() << "[*] callinst Argument value  = " << function->getName() << "\n";
          ParseFunction_Ret(function);
          // for (Function::iterator bi = function->begin(), be = function->end(); bi != be; bi++){
          //   for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++){
          //     Instruction *inst = dyn_cast<Instruction>(ii);
          //     if(ReturnInst *RI = dyn_cast<ReturnInst>(inst)){
          //       Value *Rvalue = RI->getReturnValue();
          //       if (CallInst *callinst = dyn_cast<CallInst>(Rvalue)){
          //         Value *value = callinst->getOperand(index);
          //         if (Argument *argument = dyn_cast<Argument>(value))
          //           ParseArgument(argument);
          //       }
          //     }
          //   }
          // }

        }else{
        // call节点与自己相等，直接解析，自己递归
        errs() << "[*] callinst Argument value  = " << value->getName() << "\n";
        ParseValue(value);  
        }
      }else if (PHINode *phiNode = dyn_cast<PHINode>(user)){ // 如果这个使用的过程变量不是一个callinst，而是一个phiNode
        errs() << "[*] That is phiNode  in ParseArgument.\n";
        errs() << "[*] ParseArgument phiNode name = " << phiNode->getName() << "\n"; // call
        for (User* user : phiNode->users()) {
          errs() << "[*] ParseArgument pHINode user = " << user->getName() << "\n"; // call
          if (CallInst* callInst = dyn_cast<CallInst>(user)) {
            Value* value = callInst->getOperand(index);//拿到对应index的value
            errs() << "[*] ParseArgument pHINode value = " << value->getName() << "\n"; // call
            ParseValue(value);
          }
        }
      }
    }
  }

  void ParseValue(Value *value){
    if (PHINode *phiNode = dyn_cast<PHINode>(value))
    {
      ParsePhiNode(phiNode);
    }
    else if (Function *func = dyn_cast<Function>(value))
    {
      errs() << "[+] Push_Function = " << func->getName() << "\n";
      Push_funcList(func->getName());
    }
    else if (Argument *argument = dyn_cast<Argument>(value))
    {
      errs() << "[*] ParseValue Argument   = " << argument->getName() << "\n";
      ParseArgument(argument);
    }
  }

  void ParseCallInst(CallInst *callinst){
    errs() << "[*] ParseCallInst callinst = " << callinst->getName() << '\n';
    if(callinst->getCalledFunction()){// 返回调用的函数，如果这是间接函数调用，则返回null
      errs() << "if true.\n";
      std::string func_name = callinst->getCalledFunction()->getName();
      errs() << func_name << "\n";
      ParseFunction_Ret(callinst->getCalledFunction());
    }else{
      // callinst不是直接调用
      errs() << "else \n";
      Value *value = callinst->getCalledValue();
      errs() << "[*] getCalledValue in parsecallinst = " << value->getName() << '\n';
      if (PHINode *phiNode = dyn_cast<PHINode>(value))
      {
        errs() << "[*] That is phiNode  in parsecallinst.\n";
        for(Value* inComingVal : phiNode->incoming_values()){
          if(Function* func = dyn_cast<Function>(inComingVal)){
            ParseFunction_Ret(func);
          }
        }
      }
      else if (Argument *argument = dyn_cast<Argument>(value))//调用的是个函数
      {
        errs() << "[*] That is Argument  in parsecallinst.\n";
        int index = argument->getArgNo();
        Value* tmp_value = callinst->getArgOperand(index);
        argument = dyn_cast<Argument>(tmp_value);
        ParseArgument(argument);
      }
      else if (CallInst *callInst = dyn_cast<CallInst>(value)) //调用给定的函数
      {
        errs() << "[*] That is CallInst  in parsecallinst.\n";
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

