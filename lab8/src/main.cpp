#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "Executor.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_LLVM_IR_file>\n";
        return 1;
    }

    // Get the file path from user input
    const char* filePath = argv[1];
    llvm::LLVMContext context;
    llvm::SMDiagnostic err;

    // Load the LLVM IR file
    auto module = llvm::parseIRFile(filePath, err, context);
    if (!module) {
        err.print(argv[0], llvm::errs());
        return 1;
    }
    std::cout << GREEN_TEXT("LLVM IR file loaded successfully") << std::endl;


    // Create the executor to interpret the program
    Executor executor(std::move(module));
    auto mainFunc = executor.module->getFunction("main");
    executor.runFunctionAsMain(mainFunc);

    return 0;
}
