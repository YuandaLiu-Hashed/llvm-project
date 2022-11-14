//===-- Scott8FrameLowering.h - Define frame lowering for Scott8 --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_Scott8FRAMELOWERING_H
#define LLVM_LIB_TARGET_Scott8_Scott8FRAMELOWERING_H

#include "Scott8.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class Scott8Subtarget;
class Scott8FrameLowering : public TargetFrameLowering {
public:
  explicit Scott8FrameLowering(const Scott8Subtarget &ST);

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const override;

  bool hasFP(const MachineFunction &MF) const override;

  bool canSimplifyCallFramePseudos(const MachineFunction &MF) const override;
private:
};

} // End llvm namespace

#endif
