#include "Scott8TargetMachine.h"
#include "Scott8.h"
// #include "Scott8TargetObjectFile.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"

#include <iostream>

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

//===----------------------------------------------------------------------===//
// Expand PostRA Psudo 
//===----------------------------------------------------------------------===//

class ExpandPostRAPsudoPass : public MachineFunctionPass {
public:
  ExpandPostRAPsudoPass() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "Expand PostRA Pass"; }

  bool runOnMachineFunction(MachineFunction &MF) override;

  bool expandSub(MachineInstr *MI, const Scott8InstrInfo *TII);
  bool expandUMul(MachineInstr *MI, const Scott8InstrInfo *TII);

  bool expandShiftRI(MachineInstr *MI, const Scott8InstrInfo *TII, bool isLeft);

  bool expandShiftRR(MachineInstr *MI, const Scott8InstrInfo *TII, bool isLeft);

private:
  static char ID;
};

bool ExpandPostRAPsudoPass::runOnMachineFunction(MachineFunction &MF) {
  const Scott8Subtarget *STI = &MF.getSubtarget<Scott8Subtarget>();
  const Scott8InstrInfo *TII = STI->getInstrInfo();
  DebugLoc DL;
  bool Expanded = false;

  MachineFunction::iterator MFI = MF.begin();
  while (MFI != MF.end()) {
    MachineFunction::iterator cMFI = MFI++;

    MachineBasicBlock &MBB = *cMFI;

    MachineBasicBlock::iterator MBBI = MBB.begin();
    while (MBBI != MBB.end()) {
      MachineBasicBlock::iterator cMBBI = MBBI++;
      bool expanded = false;
      switch (cMBBI->getOpcode()) {
        case Scott8::SUB_PSEUDO:
          expanded = expandSub(cMBBI.operator->(), TII);
          break;
        case Scott8::UMUL_PSEUDO:
          expanded = expandUMul(cMBBI.operator->(), TII);
          break;

        case Scott8::SHLri_PSEUDO:
          expanded = expandShiftRI(cMBBI.operator->(), TII, /* isLeft */ true);
          break;
        case Scott8::SHRri_PSEUDO:
          expanded = expandShiftRI(cMBBI.operator->(), TII, /* isLeft */false);
          break;
        case Scott8::SHLrr_PSEUDO:
          expanded = expandShiftRR(cMBBI.operator->(), TII, /* isLeft */true);
          break;
        case Scott8::SHRrr_PSEUDO:
          expanded = expandShiftRR(cMBBI.operator->(), TII, /* isLeft */false);
          break;
      }
      if (expanded) {
        MFI = MF.begin();
        break;
      }
    }
  }
  return Expanded;
}

// Expand SUB
bool ExpandPostRAPsudoPass::expandSub(MachineInstr *MI, const Scott8InstrInfo *TII) {
  MachineBasicBlock *MBB = MI->getParent();
  DebugLoc DL;

  const MachineOperand &Minuend = MI->getOperand(0);
  const MachineOperand &Subtrahend = MI->getOperand(2);

  BuildMI(*MBB, MI, DL, TII->get(Scott8::CPYri))
      .addReg(Scott8::TmpReg)
      .addImm(1);
  BuildMI(*MBB, MI, DL, TII->get(Scott8::NOTrr))
      .addDef(Subtrahend.getReg())
      .addUse(Subtrahend.getReg(), RegState::Kill);
  BuildMI(*MBB, MI, DL, TII->get(Scott8::ADDrr))
      .addDef(Scott8::TmpReg)
      .addUse(Scott8::TmpReg, RegState::Kill)
      .addUse(Subtrahend.getReg(), RegState::Kill);
  BuildMI(*MBB, MI, DL, TII->get(Scott8::CLF));
  BuildMI(*MBB, MI, DL, TII->get(Scott8::ADDrr))
      .addDef(Minuend.getReg())
      .addUse(Minuend.getReg(), RegState::Kill)
      .addUse(Scott8::TmpReg, RegState::Kill);
  BuildMI(*MBB, MI, DL, TII->get(Scott8::CLF));

  if (!Subtrahend.isKill()) {
    BuildMI(*MBB, MI, DL, TII->get(Scott8::NOTrr))
        .addDef(Subtrahend.getReg())
        .addUse(Subtrahend.getReg(), RegState::Kill);
  }

  MI->eraseFromParent();

  return false;
}

// Expand UMUL
bool ExpandPostRAPsudoPass::expandUMul(MachineInstr *MI, const Scott8InstrInfo *TII) {
  MachineBasicBlock *MBB = MI->getParent();
  DebugLoc DL;

  const MachineOperand &Lhs = MI->getOperand(2);
  const MachineOperand &Rhs = MI->getOperand(0);

  //Shift RHS to the right by one
  //if overflow
  //  LHS + TMP -> TMP
  //Shift LHS to the left by one
  //if LHS is zero
  // end
  //if not
  // go to begin

  //  XOR TMP, TMP
  //Begin:
  //  SHR Rhs, Rhs
  //  JC @A
  //  JZ @END
  //  JMP @B
  //A:
  //  CLF
  //  ADD LHS, TMP
  //B:
  //  SHL Lhs, Lhs
  //  CLF
  //  JMP @Begin
  //END:
  //  CLF

  //Start STATE: LHS (input 1) | RHS (input 2) | TMP (output)
  //END STATE: LHS (unknown) | RHS (input 2) | TMP (output)

  auto clrTmp = BuildMI(*MBB, MI, DL, TII->get(Scott8::XORrr))
      .addDef(Scott8::TmpReg)
      .addUse(Scott8::TmpReg, RegState::Kill)
      .addUse(Scott8::TmpReg, RegState::Kill);
  auto MBB_Begin = MBB->splitAt(*clrTmp, true);
  BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::SHR))
      .addDef(Rhs.getReg())
      .addUse(Rhs.getReg(), RegState::Kill);
  auto jc_A = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JCC));
      //Awaiting Completion
      //we can only complete a jump instruction after the MBB it's jumping to is created.
  auto jz_End = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JCC));
      //Awaiting Completion
  auto jmp_B = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JMP));
      //Awaiting Completion
  auto MBB_A = MBB_Begin->splitAt(*jmp_B, true);
  BuildMI(*MBB_A, MI, DL, TII->get(Scott8::CLF));
  auto add1 = BuildMI(*MBB_A, MI, DL, TII->get(Scott8::ADDrr))
      .addDef(Scott8::TmpReg)
      .addUse(Scott8::TmpReg, RegState::Kill)
      .addUse(Lhs.getReg(), RegState::Kill);
  auto MBB_B = MBB_A->splitAt(*add1, true);
  BuildMI(*MBB_B, MI, DL, TII->get(Scott8::SHL))
      .addDef(Lhs.getReg())
      .addUse(Lhs.getReg(), RegState::Kill);
  BuildMI(*MBB_B, MI, DL, TII->get(Scott8::CLF));
  auto jmp_Begin = BuildMI(*MBB_B, MI, DL, TII->get(Scott8::JMP));
      //Awaiting Completion
  auto MBB_End = MBB_B->splitAt(*jmp_Begin, true);
  BuildMI(*MBB_End, MI, DL, TII->get(Scott8::CLF));
  
  BuildMI(*MBB_End, MI, DL, TII->get(Scott8::ORrr))
      .addDef(Rhs.getReg())
      .addUse(Rhs.getReg(), RegState::Kill)
      .addUse(Scott8::TmpReg, RegState::Kill);

  //don't need to kill Rhs register, as it will be zero anyways.

  //Fill in jump target and constraints.
  jz_End.addMBB(MBB_End).addImm(0b0001);
  jc_A.addMBB(MBB_A).addImm(0b1000);
  jmp_B.addMBB(MBB_B);
  jmp_Begin.addMBB(MBB_Begin);
  //Set emit all Building Blocks
  MBB_Begin->setLabelMustBeEmitted();
  MBB_A->setLabelMustBeEmitted();
  MBB_B->setLabelMustBeEmitted();
  MBB_End->setLabelMustBeEmitted();

  MI->eraseFromParent();
  return true;
}

// Expand Shift RI
bool ExpandPostRAPsudoPass::expandShiftRI(MachineInstr *MI, const Scott8InstrInfo *TII, bool isLeft) {
  MachineBasicBlock *MBB = MI->getParent();
  DebugLoc DL;

  const Register SourceReg = MI->getOperand(1).getReg();
  const int Amount = MI->getOperand(2).getImm();
  const Register DestReg = MI->getOperand(0).getReg();

  //if shift more than 8, just set to zero.
  //This should be guarded before anyway.
  if (Amount >= 8) {
    BuildMI(*MBB, MI, DL, TII->get(Scott8::XORrr))
        .addDef(DestReg)
        .addUse(DestReg, RegState::Kill)
        .addUse(SourceReg, RegState::Kill);
    BuildMI(*MBB, MI, DL, TII->get(Scott8::CLF));
    MI->eraseFromParent();
    return true;
  }
  //Insert shifts
  //The first shift also serve as copy
  for (int i = 0; i < Amount; i++) {
    BuildMI(*MBB, MI, DL, TII->get(isLeft ? Scott8::SHL : Scott8::SHR))
        .addDef(DestReg)
        .addUse(i == 0 ? SourceReg : DestReg, RegState::Kill);
    BuildMI(*MBB, MI, DL, TII->get(Scott8::CLF));
  }
  MI->eraseFromParent();
  return true;
}

// Expand ShiftRR
bool ExpandPostRAPsudoPass::expandShiftRR(MachineInstr *MI, const Scott8InstrInfo *TII, bool isLeft) {
  MachineBasicBlock *MBB = MI->getParent();
  DebugLoc DL;

  const int AmountReg = MI->getOperand(2).getReg();
  const Register DestReg = MI->getOperand(0).getReg();

  //  DATA Tmp, -1
  //Begin:
  //  OR AmountReg, AmountReg
  //  JZ @End
  //  ADD Tmp, AmountReg
  //  CLF
  //  Shift DestReg, DestReg
  //  JZ @End
  //  JMP Begin
  //End:
  //  CLF

  auto loadNeg1 = BuildMI(*MBB, MI, DL, TII->get(Scott8::CPYri))
      .addReg(Scott8::TmpReg)
      .addImm(0xFF); //this is -1
  auto MBB_Begin = MBB->splitAt(*loadNeg1, true);
  BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::ORrr))
      .addDef(AmountReg)
      .addUse(AmountReg, RegState::Kill)
      .addUse(AmountReg, RegState::Kill);
  auto jz_End = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JCC));
      //completed later
  BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::ADDrr))
      .addDef(AmountReg)
      .addUse(AmountReg, RegState::Kill)
      .addUse(Scott8::TmpReg, RegState::Kill);
  BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::CLF));
  BuildMI(*MBB_Begin, MI, DL, TII->get(isLeft ? Scott8::SHL : Scott8::SHR))
      .addDef(DestReg)
      .addUse(DestReg, RegState::Kill);
  auto jz_End2 = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JCC));
      //completed later
  auto jmp = BuildMI(*MBB_Begin, MI, DL, TII->get(Scott8::JMP))
      .addMBB(MBB_Begin);
  auto MBB_End = MBB_Begin->splitAt(*jmp, true);
  BuildMI(*MBB_End, MI, DL, TII->get(Scott8::CLF));

  jz_End.addMBB(MBB_End).addImm(0b0001); //opcode for zero
  jz_End2.addMBB(MBB_End).addImm(0b0001);

  MBB_Begin->setLabelMustBeEmitted();
  MBB_End->setLabelMustBeEmitted();

  MI->eraseFromParent();

  return true;
}

char ExpandPostRAPsudoPass::ID = 0;

//===----------------------------------------------------------------------===//

namespace llvm {

FunctionPass *createBundleCmpJccPass() { return new BundleCmpJccPass(); }
FunctionPass *createInsertClfPass() { return new InsertClfPass(); }
FunctionPass *createInsertInitialStackStatePass() { return new InsertInitialStackStatePass(); }
FunctionPass *createExpandPostRAPseudoPass() { return new ExpandPostRAPsudoPass(); }

} // end of namespace llvm