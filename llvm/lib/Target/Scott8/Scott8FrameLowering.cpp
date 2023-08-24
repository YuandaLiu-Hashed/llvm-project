//===-- Scott8FrameLowering.cpp - Scott8 Frame Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Scott8 implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "Scott8FrameLowering.h"
#include "Scott8InstrInfo.h"
#include "Scott8MachineFunctionInfo.h"
#include "Scott8Subtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;


Scott8FrameLowering::Scott8FrameLowering(const Scott8Subtarget &ST)
    : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, /*StackAlignment=*/Align(1), 0) {}

void Scott8FrameLowering::emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  // Compute the stack size, to determine if we need a prologue at all.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  const Scott8MachineFunctionInfo *MFI = MF.getInfo<Scott8MachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t StackSize = MFI->calculateStackSize();
  if (!StackSize) {
    return;
  }

  // Adjust the stack register.
  unsigned StackReg = Scott8::SP;
  unsigned OffsetReg = Scott8::R2;

  BuildMI(MBB, MBBI, dl, TII.get(Scott8::CPYri), OffsetReg)
    .addImm(-StackSize);

  BuildMI(MBB, MBBI, dl, TII.get(Scott8::ADDrr), StackReg)
    .addReg(OffsetReg, getKillRegState(OffsetReg))
    .addReg(OffsetReg, getKillRegState(OffsetReg))
    .setMIFlag(MachineInstr::FrameSetup);
}

void Scott8FrameLowering::emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  // Compute the stack size, to determine if we need an epilogue at all.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  const Scott8MachineFunctionInfo *MFI = MF.getInfo<Scott8MachineFunctionInfo>();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc dl = MBBI->getDebugLoc();
  uint64_t StackSize = MFI->calculateStackSize();
  if (!StackSize) {
    return;
  }

  // Restore the stack register to what it was at the beginning of the function.
  unsigned StackReg = Scott8::SP;
  unsigned OffsetReg = Scott8::R2;

  BuildMI(MBB, MBBI, dl, TII.get(Scott8::CPYri), OffsetReg)
    .addImm(StackSize);

  // Adding two positive ints, no clf required.
  BuildMI(MBB, MBBI, dl, TII.get(Scott8::NO_CLF_ADDrr), StackReg)
    .addReg(OffsetReg, getKillRegState(OffsetReg))
    .addReg(OffsetReg, getKillRegState(OffsetReg))
    .setMIFlag(MachineInstr::FrameDestroy);
}

bool Scott8FrameLowering::canSimplifyCallFramePseudos(const MachineFunction &MF) const {
  return true;
}

MachineBasicBlock::iterator Scott8FrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  return MBB.erase(I);
}

bool Scott8FrameLowering::hasFP(const MachineFunction &MF) const { return false; }

