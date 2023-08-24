#include "Scott8TargetMachine.h"
#include "Scott8.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Insert CLF Pass
//===----------------------------------------------------------------------===//
class InsertClfPass : public MachineFunctionPass {
public:
  InsertClfPass() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "Insert CLF instruction pass"; }

  bool runOnMachineFunction(MachineFunction &MF) override;
private:
  static char ID;
};

bool InsertClfPass::runOnMachineFunction(MachineFunction &MF) {
  const Scott8Subtarget *STI = &MF.getSubtarget<Scott8Subtarget>();
  const Scott8InstrInfo *TII = STI->getInstrInfo();
  bool InsertedClf = false;

  for (auto &MBB : MF) {
    MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
    
    while (MBBI != E) {
      switch (MBBI->getOpcode()) {
      // Insert CLF before CMP.
      case Scott8::CMPrr:
        BuildMI(MBB, MBBI, DebugLoc(), TII->get(Scott8::CLF));
        InsertedClf = true;
        break;
      // Insert CLF after ADDrr, SHL, SHR.
      case Scott8::ADDrr:
      case Scott8::SHL:
      case Scott8::SHR:
        BuildMI(MBB, std::next(MBBI), DebugLoc(), TII->get(Scott8::CLF));
        InsertedClf = true;
        break;
      }
      MBBI = std::next(MBBI);
    }
  }
  return InsertedClf;
}

char InsertClfPass::ID = 0;

namespace llvm {

FunctionPass *createInsertClfPass() { return new InsertClfPass(); }

} // end of namespace llvm