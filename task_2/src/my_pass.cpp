#include <llvm/ADT/StringRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>

#include "llvm/IR/CFG.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Plugins/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace llvm;

/* Так как мы пишем библиотеку, то можно выбрать анонимную область видимости */
namespace {

void enumerateBasicBlocks(Function& F) {
  auto BBCounter = 0;
  for (auto& BB : F) {
    BB.setName(F.getName().str() + ".BB" + std::to_string(BBCounter++));
    // llvm::outs() << "Assigned name: " << BB.getName().str()
    //              << " to basic block.\n";
  }
  return;
}

using VectorOfLoops = std::vector<std::pair<const BasicBlock*, const BasicBlock*>>;
const std::vector<StringRef> rpoNumbering(Function& F, VectorOfLoops& Loops) {
  std::stack<const BasicBlock*> DfsStack;
  enum VisitedState {
    NIL = 0,
    IN,
    OUT
  };
  std::unordered_map<std::string, enum VisitedState> Used;
  std::vector<StringRef>                             Traversal;

  const auto* BBStart = &F.getEntryBlock();
  DfsStack.push(BBStart);

  while (!DfsStack.empty()) {
    const auto* const BB = DfsStack.top();

    Used[BB->getName().str()] = IN;

    bool AllVisited = true;

    for (const auto* Successor : successors(BB)) {
      const auto& BBName = Successor->getName().str();
      if (Used[BBName] == NIL) {
        DfsStack.push(Successor);
        AllVisited = false;
        break;
      }
      if (Used[BBName] == IN) { Loops.push_back(std::make_pair(BB, Successor)); }
    }
    if (AllVisited) {
      Used[DfsStack.top()->getName().str()] = OUT;
      Traversal.push_back(DfsStack.top()->getName());
      DfsStack.pop();
    }
  }

  return std::vector<StringRef>(Traversal.rbegin(), Traversal.rend());
}

void countInstrInBB(const BasicBlock&                        BB,
                    std::unordered_map<std::string, size_t>& InstructionCounter) {
  for (const auto& Instr : BB) { InstructionCounter[Instr.getOpcodeName()] += 1; }
}

std::unordered_map<std::string, size_t> countInstrInFunc(Function& F) {
  std::unordered_map<std::string, size_t> InstructionCounter;

  for (const auto& BB : F) { countInstrInBB(BB, InstructionCounter); }

  return InstructionCounter;
}

void RPOTraversal(Function& F) {
  enumerateBasicBlocks(F);
  errs() << "--------- RPO Numbering ------------\n";
  VectorOfLoops          Loops;
  std::vector<StringRef> RpoTraversal = rpoNumbering(F, Loops);

  if (!Loops.empty()) {
    errs() << "---- Loops Found ----\n";
    for (const auto& [from, to] : Loops) {
      errs() << from->getName() << " -> " << to->getName() << '\n';
    }
    errs() << "---------------------\n";
  }

  for (const auto& Name : RpoTraversal) { errs() << '\t' << Name << '\n'; }

  errs() << '\n';

  return;
}

void countInstructions(Function& F) {
  enumerateBasicBlocks(F);

  errs() << "------- Instruction  Counter -------\n";

  errs() << "Function " << F.getName() << "():\n";

  for (const auto& [Instr, Cnt] : countInstrInFunc(F)) {
    errs() << '\t' << Instr << " " << Cnt << '\n';
  }

  errs() << '\n';

  return;
}

void VisitFunction(Function& F) {
  /* LLVM использует кастомные потоки вывода.
   * llvm::errs() (поток ошибок),
   * llvm::outs() (аналогичен стандартному stdout),
   * llvm::nulls() (отбрасывает весь вывод) */
  errs() << "Function name: " << F.getName() << "\n";
  errs() << "    number of arguments: " << F.arg_size() << "\n";

  return;
} /* VisitFunction */

/**
 * Структура, необходимая для регистрации своего прохода в менеджере проходов.
 */
struct MyPass : PassInfoMixin<MyPass> {
  /* PreservedAnalyses - множество анализов, которые сохраняются после данного
   * прохода, чтобы не запускать их заново.
   *
   * run() непосредственно нужен для запуска прохода.
   *
   * Так как мы просто хотим вывести: "имя функции - количество аргументов",
   * то мы возвращаем all(), что говорит о том, что ни один анализ не будет
   * испорчен */
  PreservedAnalyses run(Function&                Function,
                        FunctionAnalysisManager& AnalysisManager) {
    VisitFunction(Function);
    RPOTraversal(Function);
    countInstructions(Function);
    return (PreservedAnalyses::all());
  }

  /* По умолчанию данный проход будет пропущен, если функция помечена
   * атрибутом optnone (не производить оптимизаций над ней). Поэтому
   * необходимо вернуть true, чтобы мы могли обходить и их. (в режиме сборки
   * -O0 все функции помечены как неоптимизируемые) */
  static bool isRequired(void) { return (true); }
};
} /* namespace */

/**
 * Наш проход будет реализован в виде отдельно подключаемого плагина (расширения
 * языка). Это удобный способ расширить возможности компилятора. Например,
 * сделать поддержку своей прагмы, своей оптимизации, выдачи своего
 * предупреждения.
 *
 * PassPluginLibraryInfo - структура, задающая базовые параметры для нашего
 * плагина. Её надо составить из:
 * - Версии API (для отслеживания совместимости ABI можно
 *   использовать LLVM_PLUGIN_API_VERSION)
 * - Имя плагина
 * - Версии плагина
 * - Callback для регистрации плагина через PassBuilder
 */
bool CallBackForPipelineParser(StringRef Name, FunctionPassManager& FPM,
                               ArrayRef<PassBuilder::PipelineElement>) {
  if (Name == "MyPass") {
    FPM.addPass(MyPass());
    return (true);
  }
  else { return (false); }
} /* CallBackForPipelineParser */

void CallBackForPassBuilder(PassBuilder& PB) {
  PB.registerPipelineParsingCallback(&CallBackForPipelineParser);
} /* CallBackForPassBuider */

PassPluginLibraryInfo getMyPassPluginInfo(void) {
  uint32_t    APIversion    = LLVM_PLUGIN_API_VERSION;
  const char* PluginName    = "MyPass";
  const char* PluginVersion = LLVM_VERSION_STRING;

  PassPluginLibraryInfo info = {APIversion, PluginName, PluginVersion,
                                CallBackForPassBuilder};

  return (info);
} /* getMyPassPluginInfo */

/**
 * Интерфейс, который гарантирует, что "opt" распознаст наш проход.
 * "-passes=MyPass"
 */
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return (getMyPassPluginInfo());
} /* llvmGetPassPluginInfo */