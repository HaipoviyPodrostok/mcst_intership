struct MySCCP : PassInfoMixin<MySCCP> {

  size_t NumDeadBlocks = 0;
    size_t NumConstantsPropagated = 0;

    enum class LatticeState { UNDEF = 0, CONST, OVERDEF };

    struct ValueState {
        LatticeState LatState;
        Constant *Constant;

        ValueState() : LatState(LatticeState::UNDEF), Constant(nullptr) {}
        ValueState(LatticeState S) : LatState(S), Constant(nullptr) {}
        ValueState(class Constant *C)
            : LatState(LatticeState::CONST), Constant(C) {}
    };

    using Lattice = DenseMap<Value *, ValueState>;
    Lattice LatticeVector;
    std::stack<Value *> SsaWL;
    std::stack<BasicBlock *> CfgWL;
    DenseMap<const BasicBlock *, bool> ExecutableBlocks;

    void markEdgeExecutable(BasicBlock *From, BasicBlock *To) {
        if (!ExecutableBlocks[To]) {
            CfgWL.push(To);
        }
    }

    ValueState latticeMeet(const ValueState &LHS, const ValueState &RHS) {
        if (LHS.LatState == LatticeState::UNDEF) {
            return RHS;
        }
        if (RHS.LatState == LatticeState::UNDEF) {
            return LHS;
        }

        if (LHS.LatState == LatticeState::OVERDEF ||
            RHS.LatState == LatticeState::OVERDEF) {
            return ValueState(LatticeState::OVERDEF);
        }

        if (LHS.Constant == RHS.Constant) { // equal constants
            return LHS;
        }
        return ValueState(LatticeState::OVERDEF); // different constants
    }

    void updateValue(Value *V, const ValueState &NewState) {
        ValueState &OldState = LatticeVector[V];

        if (OldState.LatState != NewState.LatState ||
            (OldState.LatState == LatticeState::CONST &&
             OldState.Constant != NewState.Constant)) {
            OldState = NewState;
            SsaWL.push(V);
        }
    }

    void sccpInit(Function &F) {
        LatticeVector.clear();
        SsaWL = std::stack<Value *>();
        CfgWL = std::stack<BasicBlock *>();
        ExecutableBlocks.clear();

        CfgWL.push(&F.getEntryBlock());
        ExecutableBlocks[&F.getEntryBlock()] = true;

        for (inst_iterator Iit = inst_begin(F); Iit != inst_end(F); ++Iit) {
            // outs() << Iit->getOpcodeName() << " " << isa<Constant>(*Iit)
            //        << "\n";
            if (isa<Constant>(&(*Iit))) {
                LatticeVector[&(*Iit)] = ValueState(cast<Constant>(&(*Iit)));
            } else {
                LatticeVector[&(*Iit)] = ValueState(LatticeState::UNDEF);
            }
        }

        for (Argument &Arg : F.args()) {
            LatticeVector[&Arg] = ValueState(LatticeState::OVERDEF);
        }

        return;
    }

    void visitBranch(BranchInst *BInst) {
        if (BInst->isUnconditional()) {
            markEdgeExecutable(BInst->getParent(), BInst->getSuccessor(0));
            return;
        }

        auto *BCond = BInst->getCondition();
        if (Constant *CInst = ConstantFoldInstruction(
                BInst, BInst->getModule()->getDataLayout())) {
            if (auto *CI = dyn_cast<ConstantInt>(CInst)) {
                bool CondVal = CI->getZExtValue();
                markEdgeExecutable(nullptr,
                                   BInst->getSuccessor(CondVal ? 0 : 1));
            }
        }

        auto &BState = LatticeVector[BCond];

        if (BState.LatState == LatticeState::CONST) {
            if (auto *CI = dyn_cast<ConstantInt>(BCond)) {
                bool CondVal = CI->getZExtValue();
                markEdgeExecutable(nullptr,
                                   BInst->getSuccessor(CondVal ? 0 : 1));
            }
        } else {
            markEdgeExecutable(nullptr, BInst->getSuccessor(0));
            markEdgeExecutable(nullptr, BInst->getSuccessor(1));
        }
    }

    void visitPHI(PHINode *PNode) {
        ValueState NewState(LatticeState::UNDEF);

        for (size_t i = 0; i < PNode->getNumIncomingValues(); ++i) {
            const auto *IncBlock = PNode->getIncomingBlock(i);
            if (!ExecutableBlocks[IncBlock]) {
                continue;
            }

            auto *IncValue = PNode->getIncomingValue(i);
            const auto &IncValueState = LatticeVector[IncValue];

            NewState = latticeMeet(NewState, IncValueState);
            if (NewState.LatState == LatticeState::OVERDEF) {
                break;
            }
        }

        updateValue(PNode, NewState);
    }
