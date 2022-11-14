//===-- Scott8InstrInfo.h - Scott8 Instruction Information --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_Scott8_Scott8INSTRINFO_H
#define LLVM_LIB_TARGET_Scott8_Scott8INSTRINFO_H

#include "Scott8RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/MachineDominators.h"

#define GET_INSTRINFO_HEADER
#include "Scott8GenInstrInfo.inc"

namespace llvm {

class Scott8Subtarget;

struct Scott8InstrInfo : public Scott8GenInstrInfo {
private:
  const Scott8RegisterInfo RI;
  const Scott8Subtarget& Subtarget;
  virtual void anchor();

public:
  explicit Scott8InstrInfo(Scott8Subtarget &ST);

  const Scott8RegisterInfo &getRegisterInfo() const { return RI; }

  unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;

  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI,
                           Register SrcReg, bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI,
                            Register DestReg, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override;

  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                            MachineBasicBlock *&FBB,
                            SmallVectorImpl<MachineOperand> &Cond,
                            bool AllowModify) const override;

  bool isUnpredicatedTerminator(const MachineInstr &MI) const;
};

} // namespace llvm

#endif
