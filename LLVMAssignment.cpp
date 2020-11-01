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


  std::map<int, std::vector<std::string>> results;
  std::vector<std::string> funcList;
  
  void Push_funcList(std::string funcname)
  {
    if (find(funcList.begin(), funcList.end(), funcname) == funcList.end())
    {
      funcList.push_back(funcname);
    }
  }
  void PrintResult()
  {
    for (auto ii = results.begin(), ie = results.end(); ii != ie; ii++)
    {
      if (ii->second.size() == 0)
      {
        continue;
      }
      errs() << ii->first << " : ";
      for (auto ji = ii->second.begin(), je = ii->second.end() - 1; ji != je; ji++)
      {
        errs() << *ji << ", ";
      }
      errs() << *(ii->second.end() - 1) << "\n";
    }
  }
  
  bool runOnModule(Module &M) override {
    errs() << "Hello: ";
    errs().write_escaped(M.getName()) << '\n';
    M.dump();
    errs()<<"------------------------------\n";
    
    for (Module::iterator fi = M.begin(), fe = M.end(); fi != fe; fi++) // 函数列表iterator的类型定义，得到的fi是iterator定义可以继续用来定义,如果需要处理或者记录该类型的语句需要将一个iterator转换为一个类指针
    {
      // Function *func_tmp = dyn_cast<Function>(fi); //相当于Function& func_tmp = *fi; and then could -->errs() << func_tmp->getName() << '\n';
      // for basicblock in function (function list)
      for (Function::iterator bi = fi->begin(), be = fi->end(); bi != be; bi++)
      {
        // for instruction in basicblock
        for (BasicBlock::iterator ii = bi->begin(), ie = bi->end(); ii != ie; ii++)
        {
          Instruction *inst = dyn_cast<Instruction>(ii);
          if (CallInst *callInst = dyn_cast<CallInst>(inst)) {
            Function* func = callInst->getCalledFunction(); // 返回调用的函数，如果这是间接函数调用，则返回null
            unsigned int callLine = callInst->getDebugLoc().getLine();
            // func返回非null，则为直接调用
            if (func) {
              std::string func_name = func->getName();
              if(!(func_name == std::string("llvm.dbg.value"))){
                funcList.clear();
                Push_funcList(func_name);
                results.insert(std::pair<int, std::vector<std::string>>(callLine, funcList));
              }
            }else{//间接调用
              funcList.clear();
              handleFunction(callInst);
              // results.insert(std::pair<int, std::vector<std::string>>(callLine, funcList));
            }
          }
        }
      }
    }
    PrintResult();
    return false;
  }
  void handleFunction(CallInst* callInst){
    // Value　is the base class of all values
    // Return the value actually being called or invoked.
    Value* value = callInst->getCalledValue();
    errs() << value->getName()<<'\n';

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

