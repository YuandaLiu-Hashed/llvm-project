#include "Scott8TargetMachine.h"
#include "Scott8.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"

using namespace llvm;

//===----------------------------------------------------------------------===//
// Bundle Cmp Jcc Pass
//===----------------------------------------------------------------------===//
class BundleCmpJccPass : public MachineFunctionPass {
public:
  BundleCmpJccPass() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "Bundle CMP with JCC Pass"; }

  bool runOnMachineFunction(MachineFunction &MF) override;
private:
  static char ID;
};

bool BundleCmpJccPass::runOnMachineFunction(MachineFunction &MF) {
  bool BundledCmp = false;

  for (auto &MBB : MF) {
    // Glueing CMP to JCC (at DAG stage) is not enough to keep them together.
    // They need stronger bonds so that only death can separate them.
    // For some reason spilled reg loads get in the way between them (reg loads require ADD that messes up the flags register).
    //
    // I'll need to look into it as it should not happen (ADD has Defs=[FR]).
    // Probably this happens because the expansion of reg loads happens too late (during TargetFrameIndex elimination?).
    //
    // So bundling them looks like a hack, but seems to work.
    MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
    while (MBBI != E) {
      MachineBasicBlock::iterator NMBBI = std::next(MBBI);
      switch (MBBI->getOpcode()) {
      case Scott8::CMPrr:
        assert(NMBBI->getOpcode() == Scott8::JCC && "JCC should immediately follow CMP");
        NMBBI->bundleWithPred();
        BundledCmp = true;
        break;
      }
      MBBI = NMBBI;
    }
  }
  return BundledCmp;
}

char BundleCmpJccPass::ID = 0;

namespace llvm {
  FunctionPass *createBundleCmpJccPass() { return new BundleCmpJccPass(); }
}