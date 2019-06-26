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

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

#define DEBUG_TYPE "main-create-pass"
#define MAX_ARGS 27
#define ARGV_IDX 0

#define DEBUG 0

namespace llvm {
  struct MainCreate : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    MainCreate() : ModulePass(ID) {
		initializeMainCreatePass(*PassRegistry::getPassRegistry());
	} 
	
	void getAnalysisUsage(AnalysisUsage &AU) const override {
	}

    bool runOnModule(Module &M) override {

    Function* kernel_function = NULL;
	std::vector<Function*> functions;

    if (M.getTargetTriple() == "naplespu-none-none" && M.getNamedMetadata("opencl.ocl.version") ) { 
#if DEBUG
        outs() << "OpenCL C Module Found! \n";      
#endif
    }
    else {
#if DEBUG
        outs() << "OpenCL C Module NOT Found! \n";      
#endif
    }

	for (auto &F : M){
		 if(F.getName().compare("main") == 0){
            #if DEBUG
			errs() << "Main function already found! Quitting...\n"; 
            #endif // DEBUG
			return false;
		 }
#if DEBUG
		outs() << "Function Name: ";
		outs().write_escaped(F.getName()) << "\n";	
		outs() << "Argument size: " << F.arg_size() << "\n";
#endif

		functions.push_back(&F);
	}

#if DEBUG
	outs() << "Number of Functions in Module: ";
	outs() << functions.size() << " \n";
#endif

    for(auto &f : functions) {
        if(f->getCallingConv() == llvm::CallingConv::SPIR_KERNEL){
#if DEBUG
            outs () << "Kernel Function Found...\n";
#endif
            kernel_function = f;
            kernel_function->setCallingConv(llvm::CallingConv::C);
            break;
        }
    }

    if(!kernel_function) {
#if DEBUG
        outs() << "No kernel function found! \n";
#endif
        return false;
    }

    LLVMContext& ctx = M.getContext();
	
#if DEBUG	
	Function::arg_iterator arg_b = kernel_function->arg_begin();
	Function::arg_iterator arg_e = kernel_function->arg_end();
	Type* type;

	while (arg_b != arg_e) {
	
		outs() << "Argument Type: ";
		type = (*arg_b).getType();
		type->print(errs());
		outs() << "\n";
		arg_b++;

	}	
#endif
	
   IRBuilder<> builder(ctx);  
   
   std::vector<Type*> ArgTypes{builder.getInt32Ty(), builder.getVoidTy()->getPointerTo()->getPointerTo()};
   
   FunctionType *FT = FunctionType::get(builder.getVoidTy(), ArgTypes, false);

   Function* main = Function::Create(FT, Function::ExternalLinkage, "main", M);
	
    BasicBlock *block = BasicBlock::Create(ctx, "entry", main, 0);
	builder.SetInsertPoint(block);
	
	std::vector<Value*> kernel_arguments;
	uint64_t index = ARGV_IDX;
	
    auto main_args = main->args();

    Argument* argv;

    for(Argument &a : main_args){
        if (a.getType()->isPointerTy()) {
            argv = &a;
#if DEBUG
            outs() << "Argv Found\n";
#endif
        }
    }

	LoadInst* argument_list[MAX_ARGS];
	unsigned counter = 0;

	for (Argument &arg : kernel_function->args()){
		argument_list[counter] = builder.CreateLoad(arg.getType(), builder.CreateBitCast(builder.CreateGEP(argv, builder.getInt32(index)), arg.getType()->getPointerTo()));
		argument_list[counter]->setAlignment(4); 
		kernel_arguments.push_back(argument_list[counter++]);//  (const char*) "...")); 
		
		index++;

	}	

	builder.CreateCall(kernel_function, kernel_arguments);

      	builder.CreateRetVoid();
      	return true;
    
}

};

}

char MainCreate::ID= 0;

//No dependecies
//IR Modification
INITIALIZE_PASS_BEGIN(MainCreate, "maincreate", "maincreate", true, false)
INITIALIZE_PASS_END(MainCreate, "maincreate", "maincreate", true, false)

ModulePass *llvm::createMainCreatePass() {
	 return new MainCreate(); 
}
