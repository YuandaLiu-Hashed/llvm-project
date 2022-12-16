//===-- Scott8RegisterInfo.cpp - Scott8 Register Information ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the Scott8 implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "Scott8RegisterInfo.h"
#include "Scott8.h"
#include "Scott8MachineFunctionInfo.h"
#include "Scott8Subtarget.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "Scott8GenRegisterInfo.inc"

Scott8RegisterInfo::Scott8RegisterInfo() : Scott8GenRegisterInfo(Scott8::PC) {}

const MCPhysReg *Scott8RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CC_Save_SaveList;
}

const uint32_t *Scott8RegisterInfo::getCallPreservedMask(const MachineFunction &MF, CallingConv::ID) const {
  return CC_Save_RegMask;
}

BitVector Scott8RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());

  Reserved.set(Scott8::SP);
  Reserved.set(Scott8::FR);
  Reserved.set(Scott8::PC);

  return Reserved;
}

const TargetRegisterClass *Scott8RegisterInfo::getPointerRegClass(const MachineFunction &MF, unsigned Kind) const {
  return &Scott8::GPRegsRegClass;
}

void Scott8RegisterInfo::eliminateFrameIndexInAdd(MachineBasicBlock::iterator II, unsigned FIOperandNum, int Offset) const
{
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  Register ReturnValueReg = MI.getOperand(0).getReg();
  Register FrameReg = getFrameRegister(MF);
  const Scott8InstrInfo *TII = MF.getSubtarget<Scott8Subtarget>().getInstrInfo();

  BuildMI(*MI.getParent(), II, DebugLoc(), TII->get(Scott8::CPYri), ReturnValueReg)
    .addImm(Offset);

  MI.getOperand(FIOperandNum).ChangeToRegister(ReturnValueReg, false, false, false);
  MI.getOperand(FIOperandNum + 1).setReg(FrameReg);
}

void Scott8RegisterInfo::eliminateFrameIndexInStore(MachineBasicBlock::iterator II, unsigned FIOperandNum, int Offset) const
{
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineBasicBlock &MBB = *MI.getParent();
  Register RegisterToStore = MI.getOperand(0).getReg();
  Register FrameReg = getFrameRegister(MF);
  const Scott8InstrInfo *TII = MF.getSubtarget<Scott8Subtarget>().getInstrInfo();

  BuildMI(MBB, II, DebugLoc(), TII->get(Scott8::CPYri), Scott8::TmpReg)
    .addImm(Offset);

  BuildMI(MBB, II, DebugLoc(), TII->get(Scott8::NO_CLF_ADDrr), Scott8::TmpReg)
    .addReg(Scott8::TmpReg)
    .addReg(FrameReg);

  MI.getOperand(0).setReg(RegisterToStore);
  MI.getOperand(FIOperandNum).ChangeToRegister(Scott8::TmpReg, false, false, true);
}

void Scott8RegisterInfo::eliminateFrameIndexInLoad(MachineBasicBlock::iterator II, unsigned FIOperandNum, int Offset) const
{
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineBasicBlock &MBB = *MI.getParent();
  Register FrameReg = getFrameRegister(MF);
  const Scott8InstrInfo *TII = MF.getSubtarget<Scott8Subtarget>().getInstrInfo();
  Register RegisterToLoad = MI.getOperand(0).getReg();

  BuildMI(MBB, II, DebugLoc(), TII->get(Scott8::CPYri), RegisterToLoad)
    .addImm(Offset);

  BuildMI(MBB, II, DebugLoc(), TII->get(Scott8::NO_CLF_ADDrr), RegisterToLoad)
    .addReg(RegisterToLoad)
    .addReg(FrameReg);

  MI.getOperand(0).setReg(RegisterToLoad);
  MI.getOperand(FIOperandNum).ChangeToRegister(RegisterToLoad, false, false, true);
}

bool Scott8RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  const Scott8MachineFunctionInfo *MFI = MF.getInfo<Scott8MachineFunctionInfo>();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  int Offset = MFI->calculateStackSlotOffset(FrameIndex);

  if (Offset < 0) {
    report_fatal_error("Negative frame index offset!");
  }

  switch (MI.getOpcode()) {
    case Scott8::NO_CLF_ADDrr:
      eliminateFrameIndexInAdd(II, FIOperandNum, Offset);
      return true;
    break;
    case Scott8::ST:
      eliminateFrameIndexInStore(II, FIOperandNum, Offset);
      return true;
    break;
    case Scott8::LD:
      eliminateFrameIndexInLoad(II, FIOperandNum, Offset);
      return true;
    break;
  }
}

Register Scott8RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return Scott8::SP;
}
