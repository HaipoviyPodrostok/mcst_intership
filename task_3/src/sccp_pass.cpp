#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/ConstantFolding.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Plugins/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>

#include <set>
#include <stack>
#include <utility>

using namespace llvm;

namespace {

struct MySCCP : PassInfoMixin<MySCCP> {
  size_t NumDeadBlocks          = 0;
  size_t NumConstantsPropagated = 0;

  // полурешетка:
  enum class LatticeState {
    UNDEF = 0,
    CONST,
    OVERDEF
  };

  struct ValueState {
    LatticeState LatState;
    Constant*    ConstVal;

    ValueState(): LatState(LatticeState::UNDEF), ConstVal(nullptr) {}
    ValueState(LatticeState S): LatState(S), ConstVal(nullptr) {}
    ValueState(Constant* C): LatState(LatticeState::CONST), ConstVal(C) {}
  };

  using Lattice = DenseMap<Value*, ValueState>;
  Lattice                           LatticeVector;
  std::stack<Instruction*>          SsaWL;
  std::stack<BasicBlock*>           CfgWL;
  DenseMap<const BasicBlock*, bool> ExecutableBlocks;
  // храним ребра а не блоки, иначе фи будет видеть значения из недостижимых  входов
  std::set<std::pair<const BasicBlock*, const BasicBlock*>> ExecutedEdges;

  ValueState getValueState(Value* V) const {
    if (auto* C = dyn_cast<Constant>(V)) {
      return ValueState(C);  // сразу возвращаем константу
    }
    auto It = LatticeVector.find(V);
    if (It != LatticeVector.end()) { return It->second; }
    return ValueState(LatticeState::UNDEF);
  }

  void markEdgeExecutable(const BasicBlock* From, const BasicBlock* To) {
    ExecutedEdges.insert({From, To});
    if (!ExecutableBlocks.count(To)) {
      ExecutableBlocks[To] = true;
      CfgWL.push(const_cast<BasicBlock*>(To));
    }
  }

  ValueState latticeMeet(const ValueState& LHS, const ValueState& RHS) const {
    if (LHS.LatState == LatticeState::UNDEF) { return RHS; }
    if (RHS.LatState == LatticeState::UNDEF) { return LHS; }

    if (LHS.LatState == LatticeState::OVERDEF ||
        RHS.LatState == LatticeState::OVERDEF)
    {
      return ValueState(LatticeState::OVERDEF);
    }

    if (LHS.ConstVal == RHS.ConstVal) { return LHS; }
    return ValueState(LatticeState::OVERDEF);
  }

  void updateValue(Value* V, const ValueState& NewState) {
    ValueState& OldState = LatticeVector[V];

    if (OldState.LatState != NewState.LatState ||
        (OldState.LatState == LatticeState::CONST &&
         OldState.ConstVal != NewState.ConstVal))
    {
      OldState = NewState;
      // при изменении состояния кидаем в SsaWL всех пользователей этого значения
      for (User* U : V->users()) {
        if (auto* UserInst = dyn_cast<Instruction>(U)) {
          if (ExecutableBlocks.count(UserInst->getParent())) {
            SsaWL.push(UserInst);
          }
        }
      }
    }
  }

  void sccpInit(Function& F) {
    LatticeVector.clear();
    SsaWL = std::stack<Instruction*>();
    CfgWL = std::stack<BasicBlock*>();
    ExecutableBlocks.clear();
    ExecutedEdges.clear();
    NumDeadBlocks          = 0;
    NumConstantsPropagated = 0;

    CfgWL.push(&F.getEntryBlock());
    ExecutableBlocks[&F.getEntryBlock()] = true;

    // аргументы функции - всегда overdef
    for (Argument& Arg : F.args()) {
      LatticeVector[&Arg] = ValueState(LatticeState::OVERDEF);
    }
  }

  void visitBranch(BranchInst* BInst) {
    if (BInst->isUnconditional()) {
      markEdgeExecutable(BInst->getParent(), BInst->getSuccessor(0));
      return;
    }

    ValueState BState = getValueState(BInst->getCondition());

    if (BState.LatState == LatticeState::CONST) {
      if (auto* CI = dyn_cast<ConstantInt>(BState.ConstVal)) {
        bool CondVal = CI->getZExtValue();
        markEdgeExecutable(BInst->getParent(), BInst->getSuccessor(CondVal ? 0 : 1));
        return;
      }
    }

    // если условие undef - не помечаем ни одно ребро, ждем пока определится
    if (BState.LatState == LatticeState::OVERDEF) {
      markEdgeExecutable(BInst->getParent(), BInst->getSuccessor(0));
      markEdgeExecutable(BInst->getParent(), BInst->getSuccessor(1));
    }
  }

  void visitPHI(PHINode* PNode) {
    ValueState NewState(LatticeState::UNDEF);

    for (size_t i = 0; i < PNode->getNumIncomingValues(); ++i) {
      const auto* IncBlock = PNode->getIncomingBlock(i);
      // берем значения только по исполняемым ребрам
      if (!ExecutedEdges.count({IncBlock, PNode->getParent()})) { continue; }

      ValueState IncValueState = getValueState(PNode->getIncomingValue(i));
      NewState                 = latticeMeet(NewState, IncValueState);
      if (NewState.LatState == LatticeState::OVERDEF) { break; }
    }

    updateValue(PNode, NewState);
  }

  void visitBinOp(Instruction* Inst) {
    ValueState LHSState = getValueState(Inst->getOperand(0));
    ValueState RHSState = getValueState(Inst->getOperand(1));

    if (LHSState.LatState == LatticeState::UNDEF ||
        RHSState.LatState == LatticeState::UNDEF)
    {
      // если хотя бы один операнд undef - ждем
      return;
    }

    if (LHSState.LatState == LatticeState::CONST &&
        RHSState.LatState == LatticeState::CONST)
    {
      SmallVector<Constant*, 2> Operands = {LHSState.ConstVal, RHSState.ConstVal};

      if (Constant* Res = ConstantFoldInstOperands(
            Inst, Operands, Inst->getModule()->getDataLayout()))
      {
        updateValue(Inst, ValueState(Res));
        return;
      }
    }

    updateValue(Inst, ValueState(LatticeState::OVERDEF));
  }

  void visitOp(Instruction* Inst) {
    switch (Inst->getOpcode()) {
      case Instruction::Br:
        visitBranch(cast<BranchInst>(Inst));
        break;
      case Instruction::PHI:
        visitPHI(cast<PHINode>(Inst));
        break;
      case Instruction::Add:
      case Instruction::Sub:
      case Instruction::Mul:
      case Instruction::ICmp:
        visitBinOp(Inst);
        break;
      case Instruction::Call:
        updateValue(Inst, ValueState(LatticeState::OVERDEF));
        break;
      default:
        updateValue(Inst, ValueState(LatticeState::OVERDEF));
        break;
    }
  }

  bool transformFunction(Function& F) {
    bool Changed = false;

    for (BasicBlock& BB : F) {
      for (Instruction& I : BB) {
        ValueState& State = LatticeVector[&I];
        if (State.LatState == LatticeState::CONST && State.ConstVal) {
          I.replaceAllUsesWith(State.ConstVal);
          NumConstantsPropagated++;
          Changed = true;
        }
      }
    }

    // заменяем условные переходы с известным условием на безусловные
    for (BasicBlock& BB : F) {
      if (!ExecutableBlocks.count(&BB)) continue;

      if (auto* BI = dyn_cast<BranchInst>(BB.getTerminator())) {
        if (BI->isConditional()) {
          ValueState BState = getValueState(BI->getCondition());
          if (BState.LatState == LatticeState::CONST) {
            if (auto* CI = dyn_cast<ConstantInt>(BState.ConstVal)) {
              bool        CondVal   = CI->getZExtValue();
              BasicBlock* AliveSucc = BI->getSuccessor(CondVal ? 0 : 1);
              BasicBlock* DeadSucc  = BI->getSuccessor(CondVal ? 1 : 0);

              // убираем эту ветку из phi узлов мертвого наследника
              DeadSucc->removePredecessor(&BB);

              // заменяем переход на безусловный
              BranchInst::Create(AliveSucc, BI);
              BI->eraseFromParent();
              Changed = true;
            }
          }
        }
      }
    }

    SmallVector<BasicBlock*, 32> DeadBlocks;
    for (BasicBlock& BB : F) {
      if (!ExecutableBlocks.count(&BB)) { DeadBlocks.push_back(&BB); }
    }

    // сначала убираем мертвые блоки из phi живых наследников, потом удаляем
    for (BasicBlock* BB : DeadBlocks) {
      for (BasicBlock* Succ : successors(BB)) {
        if (ExecutableBlocks.count(Succ)) { Succ->removePredecessor(BB); }
      }
    }

    for (BasicBlock* BB : DeadBlocks) { BB->dropAllReferences(); }

    for (BasicBlock* BB : DeadBlocks) {
      BB->eraseFromParent();
      NumDeadBlocks++;
      Changed = true;
    }

    return Changed;
  }

  PreservedAnalyses run(Function&                Function,
                        FunctionAnalysisManager& AnalysisManager) {
    sccpInit(Function);

    // сначала обходим новые блоки из CfgWL, потом пересчитываем инструкции из SsaWL
    while (!SsaWL.empty() || !CfgWL.empty()) {
      while (!CfgWL.empty()) {
        auto* BB = CfgWL.top();
        CfgWL.pop();

        for (Instruction& I : *BB) { visitOp(&I); }
      }

      if (!SsaWL.empty()) {
        auto* Inst = SsaWL.top();
        SsaWL.pop();
        if (ExecutableBlocks.count(Inst->getParent())) { visitOp(Inst); }
      }
    }

    bool Changed = transformFunction(Function);

    outs() << NumConstantsPropagated << "  " << NumDeadBlocks << "\n";

    if (!Changed) { return PreservedAnalyses::all(); }

    return PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};

}  // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    .APIVersion                   = LLVM_PLUGIN_API_VERSION,
    .PluginName                   = "My_SCCP",
    .PluginVersion                = "v0.1",
    .RegisterPassBuilderCallbacks = [](PassBuilder& PB) {
      PB.registerPipelineParsingCallback([](StringRef Name, FunctionPassManager& FPM,
                                            ArrayRef<PassBuilder::PipelineElement>) {
        if (Name == "MySCCP") {
          // mem2reg нужен чтобы привести к SSA форме до нашего пасса
          FPM.addPass(PromotePass());
          FPM.addPass(MySCCP());
          return true;
        }
        return false;
      });

      PB.registerVectorizerStartEPCallback(
        [](FunctionPassManager& FPM, OptimizationLevel Level) {
          FPM.addPass(PromotePass());
          FPM.addPass(MySCCP());
        });
    }};
}