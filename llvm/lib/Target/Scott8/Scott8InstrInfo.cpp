//===-- Scott8InstrInfo.cpp - Scott8 Instruction Information
//----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Scott8 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "Scott8InstrInfo.h"
#include "Scott8.h"
#include "Scott8MachineFunctionInfo.h"
#include "Scott8Subtarget.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "Scott8GenInstrInfo.inc"

// Pin the vtable to this file.
void Scott8InstrInfo::anchor() {}

Scott8InstrInfo::Scott8InstrInfo(Scott8Subtarget &ST)
    : Scott8GenInstrInfo(Scott8::ADJCALLSTACKDOWN, Scott8::ADJCALLSTACKUP),
      RI(), Subtarget(ST) {}

MachineBasicBlock::instr_iterator
findConditionalJumpInBundle(MachineBasicBlock::iterator I) {
  auto II = I->getIterator();
  while (II->getOpcode() != Scott8::JCC && II->isBundledWithSucc())
    II++;

  assert(II->getOpcode() == Scott8::JCC && "Expected to find JCC");
  return II;
}

unsigned Scott8InstrInfo::insertBranch(
    MachineBasicBlock &MBB, MachineBasicBlock *TBB, MachineBasicBlock *FBB,
    ArrayRef<MachineOperand> Cond, const DebugLoc &DL, int *BytesAdded) const {
  // Shouldn't be a fall through.
  assert(TBB && "insertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 1 || Cond.size() == 0) &&
         "Scott8 branch conditions have one component!");
  assert(!BytesAdded && "code size not handled");

  if (Cond.empty()) {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(Scott8::JMP)).addMBB(TBB);
    return 1;
  }

  MachineBasicBlock::const_iterator LastInstr = MBB.end();
  LastInstr--;
  assert(LastInstr->getOpcode() == Scott8::CMPrr &&
         "Expected CMP before inserting JCC");

  // Conditional branch.
  unsigned Count = 0;
  BuildMI(&MBB, DL, get(Scott8::JCC)).addMBB(TBB).addImm(Cond[0].getImm());

  ++Count;

  if (FBB) {
    // Two-way Conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(Scott8::JMP)).addMBB(FBB);
    ++Count;
  }
  return Count;
}

unsigned Scott8InstrInfo::removeBranch(MachineBasicBlock &MBB,
                                       int *BytesRemoved) const {
  assert(!BytesRemoved && "code size not handled");

  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;

    if (I->getOpcode() != Scott8::JMP && I->getOpcode() != Scott8::JCC &&
        (!I->isBundled() || !I->isBranch()))
      break;

    if (I->isBundled()) {
      auto JCC = findConditionalJumpInBundle(I);
      JCC->eraseFromBundle();
    } else {
      I->eraseFromParent();
    }

    I = MBB.end();
    ++Count;
  }

  return Count;
}

void Scott8InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, MCRegister DestReg,
                                  MCRegister SrcReg, bool KillSrc) const {
  if (Scott8::GPRegsRegClass.contains(DestReg, SrcReg)) {
    BuildMI(MBB, I, DL, get(Scott8::XORrr), DestReg)
        .addReg(DestReg)
        .addReg(DestReg);

    BuildMI(MBB, I, DL, get(Scott8::NO_CLF_ADDrr), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc))
        .addReg(SrcReg, getKillRegState(KillSrc));
  } else {
    llvm_unreachable("Impossible reg-to-reg copy");
  }
}

void Scott8InstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator MII,
                                          Register SrcReg, bool isKill, int FI,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const {
  MachineInstr &MI = *MII;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineRegisterInfo &MRI = MF.getRegInfo();

  // Emit a special psudo-instruction of store that clog a register for temporary arithmatic
  Register scratchReg = MRI.createVirtualRegister(&Scott8::NonStackGPRegsRegClass);
  BuildMI(MBB, MI, DebugLoc(), get(Scott8::STstack), scratchReg)
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FI);
}

void Scott8InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
    int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {
  BuildMI(MBB, MI, DebugLoc(), get(Scott8::LD), DestReg).addFrameIndex(FI);
}

bool Scott8InstrInfo::isUnpredicatedTerminator(const MachineInstr &MI) const {
  if (!MI.isTerminator())
    return false;

  // Conditional branch is a special case.
  if (MI.isBranch() && !MI.isBarrier())
    return true;
  if (!MI.isPredicable())
    return true;
  return !isPredicated(MI);
}

bool Scott8InstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                    MachineBasicBlock *&TBB,
                                    MachineBasicBlock *&FBB,
                                    SmallVectorImpl<MachineOperand> &Cond,
                                    bool AllowModify) const {
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();
  while (I != MBB.begin()) {
    --I;
    if (I->isDebugValue())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(*I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analysis.
    if (!I->isBranch())
      return true;

    // Handle unconditional branches.
    if (I->getOpcode() == Scott8::JMP) {
      if (!AllowModify) {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JMP, delete them.
      while (std::next(I) != MBB.end())
        std::next(I)->eraseFromParent();
      Cond.clear();
      FBB = nullptr;

      // Delete the JMP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB())) {
        TBB = nullptr;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }

      // TBB is used to indicate the unconditinal destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    unsigned Opcode = I->getOpcode();
    MachineOperand *BranchTarget = nullptr;
    MachineOperand *CondCode = nullptr;

    if (I->isBundled()) {
      auto JCC = findConditionalJumpInBundle(I);
      Opcode = JCC->getOpcode();
      BranchTarget = &JCC->getOperand(0);
      CondCode = &JCC->getOperand(1);
    } else {
      BranchTarget = &I->getOperand(0);
      CondCode = &I->getOperand(1);
    }

    // Handle conditional branches.
    assert(Opcode == Scott8::JCC && "Invalid conditional branch");
    Scott8CC::CondCodes BranchCode =
        static_cast<Scott8CC::CondCodes>(CondCode->getImm());
    if (BranchCode == Scott8CC::COND_INVALID)
      return true; // Can't handle weird stuff.

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty()) {
      FBB = TBB;
      TBB = BranchTarget->getMBB();
      Cond.push_back(MachineOperand::CreateImm(BranchCode));
      continue;
    }

    // Handle subsequent conditional branches. Only handle the case where all
    // conditional branches branch to the same destination.
    assert(Cond.size() == 1);
    assert(TBB);

    // Only handle the case where all conditional branches branch to
    // the same destination.
    if (TBB != BranchTarget->getMBB())
      return true;

    Scott8CC::CondCodes OldBranchCode = (Scott8CC::CondCodes)Cond[0].getImm();
    // If the conditions are the same, we can leave them alone.
    if (OldBranchCode == BranchCode)
      continue;

    return true;
  }

  return false;
}