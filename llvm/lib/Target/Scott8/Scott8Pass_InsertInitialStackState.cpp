#include "Scott8TargetMachine.h"
#include "Scott8.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Insert Initial Stack State Pass
//===----------------------------------------------------------------------===//
//Insert value 0xFF onto the stack at the first instruction of program execution.

class InsertInitialStackStatePass : public MachineFunctionPass {
public:
  InsertInitialStackStatePass() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "Insert Initial Stack State Pass"; }

  bool runOnMachineFunction(MachineFunction &MF) override;
private:
  static char ID;
};

bool InsertInitialStackStatePass::runOnMachineFunction(MachineFunction &MF) {
  if (MF.size() <= 0) return false;
  if (MF.getName() != "main") return false;
  const Scott8Subtarget *STI = &MF.getSubtarget<Scott8Subtarget>();
  const Scott8InstrInfo *TII = STI->getInstrInfo();
  DebugLoc DL;

  auto &MBB = MF.front();

  BuildMI(MBB, MBB.begin(), DL, TII->get(Scott8::CPYri)).addReg(Scott8::SP).addImm(0xFF);

  return true;
}

char InsertInitialStackStatePass::ID = 0;

namespace llvm {

FunctionPass *createInsertInitialStackStatePass() { return new InsertInitialStackStatePass(); }

} // end of namespace llvm