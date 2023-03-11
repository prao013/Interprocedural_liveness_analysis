
 
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatCommon.h"
#include "llvm/Support/FormatProviders.h"
#include "llvm/Support/FormatVariadicDetails.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/PassManager.h"
#include <string>
#include <sstream>
#include <map>
#include "llvm/Support/ScopedPrinter.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;

using namespace llvm;

namespace {
	set<string> killer(Function &F){
	set<string> kill;	
		for (auto& basic_block : F)
        {	
	    	
            for (auto& inst : basic_block)
            {
		if(inst.getOpcode() == Instruction::Store){
 			string operand2=inst.getOperand(1)->getName().str();
			kill.insert(operand2);
		}
	    }
				
	}
		return kill;
	}
	set<string> available(set<string> kill,Function &F)
	{
		set<string> live;	
	for (auto& basic_block : F)
        {	
	 	
            for (auto& inst : basic_block)
            {
		if(inst.getOpcode() == Instruction::Load){
 			string operand1=inst.getOperand(0)->getName().str();
 			auto it=kill.find(operand1);
		 if ( it == kill.end() )
			{live.insert(operand1);}
 		} 
	    }
	}
		return live;
	}
	
	
void visitor(Function &F){
		string func_name = "main";
        for (auto& basic_block : F)
        {	
	 	
            for (auto& inst : basic_block)
            {
		    
            } // end for inst
        } // end for block
        
}


// New PM implementation
struct ValueNumberingPass : public PassInfoMixin<ValueNumberingPass> {

  // The first argument of the run() function defines on what level
  // of granularity your pass will run (e.g. Module, Function).
  // The second argument is the corresponding AnalysisManager
  // (e.g ModuleAnalysisManager, FunctionAnalysisManager)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
  	visitor(F);
	return PreservedAnalyses::all();

	
  }
  
    static bool isRequired() { return true; }
};
}



//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "ValueNumberingPass", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
        ArrayRef<PassBuilder::PipelineElement>) {
          if(Name == "value-numbering"){
            FPM.addPass(ValueNumberingPass());
            return true;
          }
          return false;
        });
    }};
}
