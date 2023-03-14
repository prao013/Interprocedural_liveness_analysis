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
	//Function to Calculate the VARKILL of the Callee (The Flow Sensitive method)
	set<string> killer(Function &F){
	int count=0;
	map<string, set<string> > PREDBBMAP;
	map<string, set<string> > VARKILL;
	for (auto& basic_block : F)
 	{count++;
	PREDBBMAP[basic_block.getName().str()];}	
	string bbs[count];
	int i=0;
	for (auto& basic_block : F)
 	{
	bbs[i]=basic_block.getName().str();
	if(i<count){i++;}
	}	
	for (auto& basic_block : F){	
	for (auto it = pred_begin(&basic_block), et =pred_end(&basic_block); it != et; ++it){
	BasicBlock* predecessor = *it;
	PREDBBMAP[basic_block.getName().str()].insert(predecessor->getName().str());
	}}	
	
	for (auto& basic_block : F)
 	{
	for (auto& inst : basic_block)
 	{
		if(inst.getOpcode() == Instruction::Store){
		string operand2=inst.getOperand(1)->getName().str();
		VARKILL[basic_block.getName().str()].insert(operand2);
 		}	
	}}	
		
	set<string> HOLDER;
	set<string> HOLDER2;
	set<string>::iterator itr1;	
	
	for(int i=0;i<count;i++){
	if(PREDBBMAP[bbs[i]].size()>1){
	set<string>::iterator itrr;
 	for (itrr = PREDBBMAP[bbs[i]].begin();itrr != PREDBBMAP[bbs[i]].end(); itrr++)
 	{
	if(HOLDER.empty()){HOLDER=VARKILL[*itrr];}
	  else{
	set_intersection(HOLDER.begin(),HOLDER.end(), VARKILL[*itrr].begin(), VARKILL[*itrr].end(),std::inserter(HOLDER2, HOLDER2.begin()));	  
	  }
	}
	VARKILL[bbs[i]]=HOLDER2;
	HOLDER.clear();
	HOLDER2.clear();
	}
	else{ 
	set<string>::iterator itrr;
	for (itrr = PREDBBMAP[bbs[i]].begin();itrr != PREDBBMAP[bbs[i]].end(); itrr++)
 	{
	VARKILL[bbs[i]].insert(VARKILL[*itrr].begin(),VARKILL[*itrr].end());
	}
	}
	
	}	
	auto it=VARKILL.find("if.end");
	return it->second;
	}//End of Killer Function
	





	//Function to Calculate the UEVAR of the Callee Function(The Flow Sensitive method)
	set<string> availabler(Function &F)
	{
	
	int count=0;
	map<string, set<string> > PREDBBMAP;
	map<string, set<string> > UEVAR;
	map<string, set<string> > VARKILL;
	for (auto& basic_block : F)
 	{count++;}
	string bbs[count];
	for (auto& basic_block : F)
 	{count--;
	bbs[count]=basic_block.getName().str();
	}
	for (auto& basic_block : F)
 	{count++;}
	for (auto& basic_block : F){
	for (auto it = pred_begin(&basic_block), et =pred_end(&basic_block); it != et; ++it){
	BasicBlock* predecessor = *it;
	PREDBBMAP[predecessor->getName().str()].insert(basic_block.getName().str());
	}}
 	for (auto& basic_block : F)
 	{
	for (auto& inst : basic_block)
 	{
		if(inst.getOpcode() == Instruction::Store){
		string operand2=inst.getOperand(1)->getName().str();
		VARKILL[basic_block.getName().str()].insert(operand2);
 		}
		if(inst.getOpcode() == Instruction::Load){
 			string operand1=inst.getOperand(0)->getName().str();
	 		auto it=VARKILL[basic_block.getName().str()].find(operand1);
 			if ( it ==VARKILL[basic_block.getName().str()].end() )
			{UEVAR[basic_block.getName().str()].insert(operand1);}
 		}
            }
        } 	
	set<string>::iterator itrr;	
	set<string> HOLDER;
	set<string>::iterator itr4;	
	for(int i=0;i<count;i++){
		if(!PREDBBMAP[bbs[i]].empty()){
 	for (itrr = PREDBBMAP[bbs[i]].begin();itrr != PREDBBMAP[bbs[i]].end(); itrr++)
 	{
	UEVAR[bbs[i]].insert(UEVAR[*itrr].begin(),UEVAR[*itrr].end());	
	set_difference(UEVAR[bbs[i]].begin(),UEVAR[bbs[i]].end(), VARKILL[bbs[i]].begin(),VARKILL[bbs[i]].end(),std::inserter(HOLDER, HOLDER.end()));
	}
	UEVAR[bbs[i]]=HOLDER;
	HOLDER.clear();}
	}
	set<string>::iterator itr5;	
	set<string> HOLDER2;
	set_difference(UEVAR["entry"].begin(),UEVAR["entry"].end(), VARKILL["entry"].begin(),VARKILL["entry"].end(),std::inserter(HOLDER2, HOLDER2.end()));	
	return HOLDER2;
	}// End of Available Function for the Calle Function
	
	
	
	
	// This is the Function to calculate the Liveness of the Callee Function. Since this is the Context Sensitive way we cannot just recursively call Visitor Function. Currently hardcoded to fucntion "minitest()".
void visitor1(Function &F,set<string> HOLDER3){
	string func_name = "minitest1";
	int count=0;
	map<string, set<string> > PREDBBMAP;
	
	map<string, set<string> > LIVEOUT;
	map<string, set<string> > UEVAR;
	map<string, set<string> > VARKILL;
        for (auto& basic_block : F)
        {	
	if (F.getName() != func_name) {continue;}
	for (auto& basic_block : F)
 	{count++;}
	string bbs[count];
	for (auto& basic_block : F)
 	{count--;
	bbs[count]=basic_block.getName().str();
	}
	for (auto& basic_block : F)
 	{count++;}
	for (auto& basic_block : F){
	for (auto it = pred_begin(&basic_block), et =pred_end(&basic_block); it != et; ++it){
	BasicBlock* predecessor = *it;
	PREDBBMAP[basic_block.getName().str()].insert(predecessor->getName().str());
	}}
	// Calculating the UEVAR and VARKILL
 	for (auto& basic_block : F)
 	{
	for (auto& inst : basic_block)
 	{
		if(inst.getOpcode() == Instruction::Store){
		string operand2=inst.getOperand(1)->getName().str();
		VARKILL[basic_block.getName().str()].insert(operand2);
 		}
 		if(inst.getOpcode() == Instruction::Load){
 			string operand1=inst.getOperand(0)->getName().str();
	 		auto it=VARKILL[basic_block.getName().str()].find(operand1);
 			if ( it ==VARKILL[basic_block.getName().str()].end() )
			{UEVAR[basic_block.getName().str()].insert(operand1);}
 		}
		if(inst.getOpcode() == Instruction::Call){
 			Function* G= cast<CallInst>(inst).getCalledFunction();
			Function& H=*G;
		
			set<string> kill;
			set<string> available;
			kill=killer(H);
			available=availabler(H);
			set<string>::iterator itr;
  			for (itr = kill.begin();itr != kill.end(); itr++)
			{VARKILL[basic_block.getName().str()].insert(*itr);}
			set<string>::iterator itr1;
  			for (itr1 = available.begin();itr1 != available.end(); itr1++)
			{UEVAR[basic_block.getName().str()].insert(*itr1);}
 		}
            }
        }
	//Calculating the Liveness Below
	for(int i=0;i<count;i++){
		LIVEOUT[bbs[i]];
		if(i==0){LIVEOUT[bbs[i]]=HOLDER3;}
		}
	set<string>::iterator itrr;
	set<string> HOLDER;
	int i=2;
	while(i>0){
	for(int i=0;i<count;i++){
 	for (itrr = PREDBBMAP[bbs[i]].begin();itrr != PREDBBMAP[bbs[i]].end(); itrr++)
 	{
	set_difference(LIVEOUT[bbs[i]].begin(),LIVEOUT[bbs[i]].end(), VARKILL[bbs[i]].begin(),VARKILL[bbs[i]].end(),std::inserter(HOLDER, HOLDER.end()));
	HOLDER.insert(UEVAR[bbs[i]].begin(),UEVAR[bbs[i]].end());
	LIVEOUT[*itrr].insert(HOLDER.begin(),HOLDER.end());
	HOLDER.clear();
	}}
	i--;
	}
	}	
	//Printing the UEVAR and VARKILL
	for (auto& basic_block : F)
        {	
	if (F.getName() != func_name) {continue;}	
	errs() <<"\n"<< "-----"<<basic_block.getName()<<"-----"<<"\n";
	errs() <<"\n"<< "UEVAR:";
	set<string>::iterator itr;
 	for (itr = UEVAR[basic_block.getName().str()].begin();itr != UEVAR[basic_block.getName().str()].end();itr++)
 		{
 			errs() <<" "<< *itr ;
 		}
	errs() <<"\n"<< "VARKILL:";
	set<string>::iterator itr1;
 	for (itr1 = VARKILL[basic_block.getName().str()].begin(); itr1 != VARKILL[basic_block.getName().str()].end(); itr1++)
 		{
		errs() <<" "<< *itr1;
		}
	errs() <<"\n"<< "LIVEOUT:";
	set<string>::iterator itr2;
 	for (itr2 = LIVEOUT[basic_block.getName().str()].begin(); itr2 != LIVEOUT[basic_block.getName().str()].end();itr2++)
 		{
 		errs() << " "<< *itr2 ;
 		}
		errs() <<"\n";
 	}
	}// End of Visitor 1
	
	//The function to calculate the Interprocedural analysis Liveness of The Test() function. Currently hardcoded to caluclate for test() fucntion only.
void visitor(Function &F){
	string func_name = "test";
	int count=0;
	map<string, set<string> > PREDBBMAP;
	map<string, set<string> > SUCCBBMAP;
	map<string, set<string> > LIVEOUT;
	map<string, set<string> > UEVAR;
	map<string, set<string> > VARKILL;
        for (auto& basic_block : F)
        {	
	if (F.getName() != func_name) {continue;}
	
	for (auto& basic_block : F)
 	{count++;}
	string bbs[count];
	for (auto& basic_block : F)
 	{count--;
	bbs[count]=basic_block.getName().str();
	}
	for (auto& basic_block : F)
 	{count++;}
		
	//Building the Precessor map
	for (auto& basic_block : F){
	for (auto it = pred_begin(&basic_block), et =pred_end(&basic_block); it != et; ++it){
	BasicBlock* predecessor = *it;
	PREDBBMAP[basic_block.getName().str()].insert(predecessor->getName().str());
		SUCCBBMAP[predecessor->getName().str()].insert(basic_block.getName().str());
	}}
	
		
	//Calculating the UEVAR and VARKILL for the test() function
	for (auto& inst : basic_block)
 	{
		if(inst.getOpcode() == Instruction::Store){
		string operand2=inst.getOperand(1)->getName().str();
		VARKILL[basic_block.getName().str()].insert(operand2);
 		}
 		if(inst.getOpcode() == Instruction::Load){
 			string operand1=inst.getOperand(0)->getName().str();
	 		auto it=VARKILL[basic_block.getName().str()].find(operand1);
 			if ( it ==VARKILL[basic_block.getName().str()].end() )
			{UEVAR[basic_block.getName().str()].insert(operand1);}
 		}
		if(inst.getOpcode() == Instruction::Call){
 			Function* G= cast<CallInst>(inst).getCalledFunction();
			Function& H=*G;
		
			set<string> kill;
			set<string> available;
			kill=killer(H);
			available=availabler(H);
			set<string>::iterator itr;
  			for (itr = kill.begin();itr != kill.end(); itr++)
			{VARKILL[basic_block.getName().str()].insert(*itr);}
			set<string>::iterator itr1;
  			for (itr1 = available.begin();itr1 != available.end(); itr1++)
			{UEVAR[basic_block.getName().str()].insert(*itr1);}
 		}
            } 
		
	
	
	// Calculating the Liveness
	for(int i=0;i<count;i++){
		LIVEOUT[bbs[i]];
		}
	set<string>::iterator itrr;
	set<string> HOLDER;
	int i=2;
	while(i>0){
	for(int i=0;i<count;i++){
 	for (itrr = PREDBBMAP[bbs[i]].begin();itrr != PREDBBMAP[bbs[i]].end(); itrr++)
 	{
	set_difference(LIVEOUT[bbs[i]].begin(),LIVEOUT[bbs[i]].end(), VARKILL[bbs[i]].begin(),VARKILL[bbs[i]].end(),std::inserter(HOLDER, HOLDER.end()));
	HOLDER.insert(UEVAR[bbs[i]].begin(),UEVAR[bbs[i]].end());
	LIVEOUT[*itrr].insert(HOLDER.begin(),HOLDER.end());
	HOLDER.clear();
	}}
	i--;
	}
	}
	
	//Calculating and calling the function to calculate the Liveness for the calle function
	for (auto& basic_block : F){
	for (auto& inst : basic_block){
	if(inst.getOpcode() == Instruction::Call){
	if (F.getName() == func_name){
	errs()<<"\n"<<"----Liveness Analysis for the call inside the "<<basic_block.getName().str()<<" Block:----"<<"\n";
	
	Function* G= cast<CallInst>(inst).getCalledFunction();
	Function& H=*G;
	set<string> HOLDER3;
	set<string> HOLDER4;
			set<string>::iterator itrr1;
			for (itrr1 = SUCCBBMAP[basic_block.getName().str()].begin();itrr1 != SUCCBBMAP[basic_block.getName().str()].end(); itrr1++)
 	{
				set_difference(LIVEOUT[*itrr1].begin(),LIVEOUT[*itrr1].end(), VARKILL[*itrr1].begin(),VARKILL[*itrr1].end(),std::inserter(HOLDER4, HOLDER4.end()));
	HOLDER4.insert(UEVAR[*itrr1].begin(),UEVAR[*itrr1].end());
				HOLDER3.insert(HOLDER4.begin(),HOLDER4.end());
				HOLDER4.clear();
	}		
	visitor1(H,HOLDER3);	
	}}}}	
	
	//Printing the IPA Liveness for the test() function
	if (F.getName() == func_name){	
	errs()<<"\n"<<"----Interprocedural Analysis for the Test():----"<<"\n";	}
	for (auto& basic_block : F)
       {	
	if (F.getName() != func_name) {continue;}	
	
	errs() <<"\n"<< "-----"<<basic_block.getName()<<"-----"<<"\n";
	errs() <<"\n"<< "UEVAR:";
	set<string>::iterator itr;
 	for (itr = UEVAR[basic_block.getName().str()].begin();itr != UEVAR[basic_block.getName().str()].end();itr++)
 		{
 			errs() <<" "<< *itr ;
 		}
	errs() <<"\n"<< "VARKILL:";
	set<string>::iterator itr1;
 	for (itr1 = VARKILL[basic_block.getName().str()].begin(); itr1 != VARKILL[basic_block.getName().str()].end(); itr1++)
 		{
		errs() <<" "<< *itr1;
		}
	errs() <<"\n"<< "LIVEOUT:";
	set<string>::iterator itr2;
 	for (itr2 = LIVEOUT[basic_block.getName().str()].begin(); itr2 != LIVEOUT[basic_block.getName().str()].end();itr2++)
 		{
 		errs() << " "<< *itr2 ;
 		}
	errs() <<"\n";	
	}
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
