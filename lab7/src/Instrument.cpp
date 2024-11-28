#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Value.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <set>
#include "Instrument.h"

using namespace llvm;

namespace instrument{

	/**
	 * @brief Instruments a truncation instruction by inserting a runtime call to TruncInstProcess.
	 *
	 * @param I The instruction to be instrumented, expected to be a TruncInst.
	 * @param destStr The name of the destination operand.
	 * @param srcStr The name of the source operand.
	 */
	void instrumentTrunc(Instruction &I,std::string destStr,std::string srcStr) {
    	if (auto *truncInst = dyn_cast<TruncInst>(&I)) {

			/**
			* TODO: Add code to instrumentation TruncInst.
			* View the TruncInstProcess' definition from /lab7/lib/runtime.cpp
			*/

			

			
		}
	}
    
	/**
	 * @brief Checks if all index offsets of a GetElementPtrInst are zero.
	 * 
	 * Check whether the offset of the GEP instruction is 0;
	 * If it is 0, the source and destination addresses are exactly the same.
	 * 
	 * @param gepInst The GetElementPtrInst to check.
	 * @return True if all offsets are zero, false otherwise.
	 */
    bool isGepOffsetZero(GetElementPtrInst *gepInst) {
		for (unsigned i = 1; i < gepInst->getNumOperands(); ++i) {
		    Value *index = gepInst->getOperand(i);
		    
		    if (ConstantInt *constIndex = dyn_cast<ConstantInt>(index)) {
		        if (constIndex->getZExtValue() != 0) {
		            return false; 
		        }
		    } else {
		        return false;
		    }
		}
		return true; 
	}
	
	/**
	 * @brief Instruments a GetElementPtr instruction by inserting a runtime call to GEPInstProcess.
	 *
	 * @param I The instruction to be instrumented, expected to be a GetElementPtrInst.
	 * @param destStr The name of the destination operand.
	 * @param srcStr The name of the source operand.
	 */
    void instrumentGEP(Instruction &I,std::string destStr,std::string srcStr) {
    	if (auto *GEPInst = dyn_cast<GetElementPtrInst>(&I)) {
			Value *src = GEPInst->getPointerOperand(); 
			Value *dest = GEPInst;

			Module *M = GEPInst->getModule();
			LLVMContext &context = M->getContext();

			auto GEPInstProcess = M->getOrInsertFunction(
				"GEPInstProcess",
				Type::getVoidTy(context),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt1Ty(context)
				);

			IRBuilder<> builder(GEPInst);
			builder.SetInsertPoint(GEPInst->getNextNode());
			
			Value *destString = builder.CreateGlobalStringPtr(destStr);
			Value *srcString = builder.CreateGlobalStringPtr(srcStr);

			Value *castedSrc = builder.CreateBitCast(src, Type::getInt8Ty(context)->getPointerTo());
			Value *castedDest = builder.CreateBitCast(dest, Type::getInt8Ty(context)->getPointerTo());
			
			bool offsetZero = isGepOffsetZero(GEPInst);
			Value *castedOffsetZero = builder.getInt1(offsetZero);

			builder.CreateCall(GEPInstProcess, {castedDest, castedSrc,destString,srcString,castedOffsetZero});
		}
	}
    
	/**
	 * @brief Instruments a Store instruction by inserting a runtime call to StoreInstProcess or StoreInstProcessPtr.
	 *
	 * Insert a call to either StoreInstProcess or StoreInstProcessPtr
	 * depending on the type of the value being stored (integer or pointer).
	 *
	 * @param I The instruction to be instrumented, expected to be a StoreInst.
	 * @param destStr The name of the destination operand.
	 * @param srcStr The name of the source operand.
	 */
    void instrumentStore(Instruction &I,std::string destStr,std::string srcStr) {
    	if (auto *storeIns = dyn_cast<StoreInst>(&I)) {
			Value *src = storeIns->getValueOperand(); 
			Value *dest = storeIns->getPointerOperand(); 
			
			Type *type = src->getType();
			
			Module *M = storeIns->getModule();
			LLVMContext &context = M->getContext();

			IRBuilder<> builder(storeIns);
			builder.SetInsertPoint(storeIns->getNextNode());
			
			Value *destString = builder.CreateGlobalStringPtr(destStr);
    		Value *srcString = builder.CreateGlobalStringPtr(srcStr);
			
			if (type->isIntegerTy()) {
				auto storeProcess = M->getOrInsertFunction(
					"StoreInstProcess",
					Type::getVoidTy(context),
					Type::getInt8Ty(context)->getPointerTo(),
					Type::getInt8Ty(context)->getPointerTo(),
					Type::getInt8Ty(context)->getPointerTo()
					);
				
				Value *castedDest = builder.CreateBitCast(dest, Type::getInt8Ty(context)->getPointerTo());

				builder.CreateCall(storeProcess, {castedDest, destString,srcString});
			} else if (type->isPointerTy()) { 
				auto storeProcess = M->getOrInsertFunction(
					"StoreInstProcessPtr",
					Type::getVoidTy(context),
					Type::getInt8Ty(context)->getPointerTo(),
					Type::getInt8Ty(context)->getPointerTo(),
					Type::getInt8Ty(context)->getPointerTo(),
					Type::getInt8Ty(context)->getPointerTo()
					);
				
				Value *castedSrc = builder.CreateBitCast(src, Type::getInt8Ty(context)->getPointerTo());
				Value *castedDest = builder.CreateBitCast(dest, Type::getInt8Ty(context)->getPointerTo());

				builder.CreateCall(storeProcess, {castedDest, castedSrc,destString, srcString});
			}

		}
	} 
    
	/**
	 * @brief Instruments a Load instruction by inserting a runtime call to LoadInstProcess.
	 *
	 * @param I The instruction to be instrumented, expected to be a LoadInst.
	 * @param destStr The name of the destination operand.
	 * @param srcStr The name of the source operand.
	 */
    void instrumentLoad(Instruction &I,std::string destStr,std::string srcStr) {
    	if (auto *loadIns = dyn_cast<LoadInst>(&I)) {
			
			/**
			* TODO: Add code to instrumentation LoadInst.
			* Use getPointerOperand() to retrieve the pointer operand for the load instruction,
 			* which represents the address being loaded from. 
			*/


		}
	}

	/**
	 * @brief Instruments a BinaryOperator instruction by inserting a runtime call to BinaryOperatorProcess.
	 * 
	 * @param I The instruction to be instrumented, expected to be a BinaryOperator.
	 * @param destStr The name of the destination operand.
	 * @param srcStr1 The name of first source operand.
	 * @param srcStr2 The name of second source operand.
	 */
    void instrumentBinaryOperator(Instruction &I,std::string destStr,std::string srcStr1,std::string srcStr2){
    	if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {

			Module *M = binOp->getModule();
			LLVMContext &context = M->getContext();
			
			auto binaryOperatorProcess = M->getOrInsertFunction(
				"BinaryOperatorProcess",
				Type::getVoidTy(context),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt8Ty(context)->getPointerTo(),
				Type::getInt8Ty(context)->getPointerTo()
				);

			IRBuilder<> builder(binOp);
			builder.SetInsertPoint(binOp->getNextNode());
			Value *destString = builder.CreateGlobalStringPtr(destStr);
			Value *srcString1 = builder.CreateGlobalStringPtr(srcStr1);
			Value *srcString2 = builder.CreateGlobalStringPtr(srcStr2);

			builder.CreateCall(binaryOperatorProcess, {destString,srcString1,srcString2});
		}
    }

	/**
	 * @brief Mark a non-pointer value as tainted by inserting a call to TaintVal.
	 *
	 * For non-pointer type value, taint it by marking its name.
	 *
	 * @param I The instruction to be instrumented.
	 * @param srcStr The name of the value who needs to be marked as tainted.
	 */
    void instrumentTaintSourceVal(Instruction &I, std::string srcStr) {
        Module *M = I.getModule();
        LLVMContext &context = M->getContext();

        auto markAsTainted = M->getOrInsertFunction("TaintVal", 
            Type::getVoidTy(context), 
            Type::getInt8Ty(context)->getPointerTo()
            ); 

        IRBuilder<> builder(&I);
        builder.SetInsertPoint(I.getNextNode());
        Value *srcString = builder.CreateGlobalStringPtr(srcStr);
        
        builder.CreateCall(markAsTainted, srcString);
    }
    
	/**
	 * @brief Mark a pointer value as tainted by inserting a call to TaintPtrVal.
	 *
	 * For pointer type value, taint it by marking its runtime-address.
	 *
	 * @param I The instruction to be instrumented.
	 * @param var The pointer value itself that needs to be marked as tainted.
	 * @param srcStr The name of the value who needs to be marked as tainted.
	 */
    void instrumentTaintSourcePtrVal(Instruction &I, Value *var, std::string id) {
        Module *M = I.getModule();
        LLVMContext &context = M->getContext();

        auto markAsTainted = M->getOrInsertFunction("TaintPtrVal", 
            Type::getVoidTy(context), 
            Type::getInt8Ty(context)->getPointerTo(),
            Type::getInt8Ty(context)->getPointerTo()
            ); 

        IRBuilder<> builder(&I);
        builder.SetInsertPoint(I.getNextNode());
        Value *castedVar = builder.CreateBitCast(var, Type::getInt8Ty(context)->getPointerTo());
        Value *idString = builder.CreateGlobalStringPtr(id);
        
        builder.CreateCall(markAsTainted, {castedVar,idString});
    }

	/**
	 * @brief Checks if a none-pointer value is tainted by inserting a call to CheckVal.
	 *
	 * If the value is tainted, it indicates that taint has propagated to a sensitive location.
	 * For non-pointer type value, it is known by its name whether it is tainted or not.
	 *
	 * @param I The instruction to be instrumented.
	 * @param srcStr The name of the value who needs to be checked.
	 */
    void instrumentTaintSinkVal(Instruction &I,std::string srcStr){
    	Module *M = I.getModule();
        LLVMContext &context = M->getContext();

        auto markAsTainted = M->getOrInsertFunction("CheckVal", 
            Type::getVoidTy(context), 
            Type::getInt8Ty(context)->getPointerTo());

        IRBuilder<> builder(&I);
        Value *srcString = builder.CreateGlobalStringPtr(srcStr);
        
        builder.CreateCall(markAsTainted, srcString);
    }

	/**
	 * @brief Checks if a pointer value is tainted by inserting a call to CheckPtrVal.
	 *
	 * For pointer type value, it is known by its address whether it is tainted or not.
	 *
	 * @param I The instruction to be instrumented.
	 * @param var The pointer value itself that needs to be checked.
	 * @param srcStr The name of the pointer value who needs to be checked.
	 */
	void instrumentTaintSinkPtrVal(Instruction &I,Value *var,std::string srcStr){
    	Module *M = I.getModule();
        LLVMContext &context = M->getContext();

        auto markAsTainted_ptr = M->getOrInsertFunction("CheckPtrVal", 
            Type::getVoidTy(context), 
            Type::getInt8Ty(context)->getPointerTo(),
            Type::getInt8Ty(context)->getPointerTo()
            ); 

        IRBuilder<> builder(&I);
        Value *srcString = builder.CreateGlobalStringPtr(srcStr);
        Value *castedVar = builder.CreateBitCast(var, Type::getInt8Ty(context)->getPointerTo());
        
        builder.CreateCall(markAsTainted_ptr, {castedVar,srcString});
    }
	
	/**
	 * @brief Instruments an instruction to propagate taint during strcat().
	 *
	 * This function is used to instrument a call to strcat function
	 * by inserting a call to StrcatProcess. It propagates taint information from the source string 
	 * to the destination string just like other instructions.
	 *
	 * @param I The instruction to be instrumented.
	 * @param dst The destination pointer value where the source string will be concatenated.
	 * @param src The source pointer value that will be concatenated to the destination.
	 * @param destStr The name of the destination pointer value.
	 * @param srcStr The name of the source pointer value.
	 */
    void instrumentForStrcat(Instruction &I, Value *dst, Value *src,std::string destStr,std::string srcStr) {
        Module *M = I.getModule();
        LLVMContext &context = M->getContext();

        auto propagateTaint = M->getOrInsertFunction("StrcatProcess", 
            Type::getVoidTy(context), 
            Type::getInt8Ty(context)->getPointerTo(),
            Type::getInt8Ty(context)->getPointerTo(),
            Type::getInt8Ty(context)->getPointerTo(), 
            Type::getInt8Ty(context)->getPointerTo()
            ); 

        IRBuilder<> builder(&I);
        builder.SetInsertPoint(I.getNextNode());
        Value *castedsrc = builder.CreateBitCast(src, Type::getInt8Ty(context)->getPointerTo());
        Value *casteddst = builder.CreateBitCast(dst, Type::getInt8Ty(context)->getPointerTo());
        Value *destString = builder.CreateGlobalStringPtr(destStr);
		Value *srcString = builder.CreateGlobalStringPtr(srcStr);
		
        builder.CreateCall(propagateTaint, {casteddst, castedsrc, destString, srcString});
    }
}