#pragma once

#include <llvm/IR/Analysis.h>
#include <llvm/IR/PassManager.h>

class SccpPass : llvm::PassInfoMixin<SccpPass>{
  llvm::PreservedAnalyses run(llvm::Function& F, llvm::FunctionAnalysisManager& AnlisysManager);
};