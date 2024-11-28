#include "llvm/Support/raw_ostream.h"
#include <set>

using namespace llvm;


/**
 * Print in color to make your output more eye-catching:
 * Green -> Source of pollution
 * Yellow -> Spread of contamination
 * Red -> Contaminated tank
 * White -> Variable log printing
 */
raw_ostream& errsYellow() {
    llvm::errs() << "\033[33m";
    return llvm::errs(); 
}

raw_ostream& errsRed() {
    llvm::errs() << "\033[31m";
    return llvm::errs(); 
}

raw_ostream& errsGreen() {
    llvm::errs() << "\033[32m";
    return llvm::errs(); 
}

raw_ostream& errsWhite() {
    llvm::errs() << "\033[0m";
    return llvm::errs(); 
}


extern "C" {
	// taintedPtrVars stores the tainted memory address(for pointer type)
	// taintedVars stores the name of the tainted variable(for non-pointer type)
	std::set<void*> taintedPtrVars;
	std::set<std::string> taintedVars;	


	void TruncInstProcess(const char *destStr,const char *srcStr){
		std::string srcString(srcStr);
		std::string destString(destStr);
		
		if(taintedVars.find(srcString)!=taintedVars.end() ){
			taintedVars.insert(destString);
		
			errsYellow()<< "Taint propagated from " << srcStr << " to " << destStr << "\n";
			errsWhite()<< "From :" << srcString <<" "<< "to :" << destString << " " << "\n";
		}
	}

	void GEPInstProcess(void *destPtr, void * srcPtr, const char *destStr, const char *srcStr,bool offsetZero){
		std::string srcString(srcStr);
		std::string destString(destStr);
		if(offsetZero && taintedPtrVars.find(srcPtr)!=taintedPtrVars.end() ){
			taintedPtrVars.insert(destPtr);
		
			errsYellow()<< "Taint propagated from " << srcPtr << " to " << destPtr << "\n";
			errsWhite()<<"From :"<<srcString<<" "<<"to :"<<destString<<" "<<"\n";
		}
	}

	void StoreInstProcess(void *destPtr,const char *destStr, const char *srcStr){
		std::string srcString(srcStr);
		std::string destString(destStr);

		/**
		 * TODO: Complete the taint propagation processing logic for StoreInst that stores non-pointer types.
		 * 
		 * You can use the following two statements to output logs:
		 * errsYellow()<< "Taint propagated from " << srcStr << " to " << destPtr << "\n";
		 * errsWhite()<<"From :"<<srcString<<" "<<"to :"<<destString<<" "<<"\n";
		 * 
		 */
		
	}

	void StoreInstProcessPtr(void *destPtr, void * srcPtr,const char *destStr, const char *srcStr){
		std::string srcString(srcStr);
		std::string destString(destStr);

		/**
		 * TODO: Complete the taint propagation processing logic for StoreInst that stores pointer types.
		 * 
		 * You can use the following two statements to output logs:
		 * errsYellow()<< "Taint propagated from " << srcPtr << " to " << destPtr << "\n";
		 * errsWhite()<<"From :"<<srcString<<" "<<"to :"<<destString<<" "<<"\n";
		 * 
		 */
		
	}

	void LoadInstProcess(void * srcPtr,const char *destStr,const char *srcStr){
		std::string srcString(srcStr);
		std::string destString(destStr);
		
		if(taintedPtrVars.find(srcPtr)!=taintedPtrVars.end() ){
			taintedVars.insert(destString);
		
			errsYellow()<< "Taint propagated from " << srcPtr << " to " << destStr << "\n";
			errsWhite()<<"From :"<<srcString<<" "<<"to :"<<destString<<" "<<"\n";
			
		}
	}

	void BinaryOperatorProcess(const char *destStr,const char *srcStr1, const char *srcStr2){
		std::string srcString1(srcStr1);
		std::string srcString2(srcStr2);
		std::string destString(destStr);

		/**
		 * TODO: Complete the taint propagation processing logic for BinaryOperator.
		 * 
		 * You can use the following two statements to output logs:
		 * errsYellow()<< "Taint propagated from " << srcString1+" , "+srcString2 << " to " << destString << "\n";
		 * errsWhite()<<"From :"<<srcString1+" , "+srcString2<<" "<<"to :"<<destString<<" "<<"\n";
		 * 
		 */
		
	}

	void TaintVal(const char *id) {
		std::string destString(id);
		taintedVars.insert(destString);
		errsGreen() << "tainted var : " << destString << "\n";
	}

	void TaintPtrVal(void * var,const char *id) {
		std::string destString(id);
		taintedPtrVars.insert(var);
		errsGreen() << "tainted var address: " << var << "\n";
		errsWhite() <<"That's the address in:"<< destString <<"\n";
	}

	void CheckVal(const char *srcStr) {
		std::string srcString(srcStr);
		if(taintedVars.find(srcStr)!=taintedVars.end()){
			errsRed() << "Taint detected in sensitive position: " << srcStr << "!!!\n";
		}
		
	}

	void CheckPtrVal(void * srcPtr,const char *srcStr) {
		std::string srcString(srcStr);
		if(taintedPtrVars.find(srcPtr)!=taintedPtrVars.end()){
			errsRed()<< "Taint detected in sensitive position: " << srcPtr << "!!!\n";
			errsWhite()<<"That's the address in:"<<srcString<<"\n";
		}
		
	}

	void StrcatProcess(void * destPtr, void * srcPtr, const char *destStr, const char *srcStr) {
		std::string srcString(srcStr);
		std::string destString(destStr);
		if(taintedPtrVars.find(srcPtr)!=taintedPtrVars.end()){
			taintedPtrVars.insert(destPtr);
			
			errsYellow()<< "Taint propagated from " << srcPtr << " to " << destPtr << "\n";
			errsWhite()<<"From :"<<srcString<<" "<<"to :"<<destString<<" "<<"\n";
		}
	}
}
