//===-- Scott8MCTargetDesc.cpp - Scott8 Target Descriptions -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Scott8 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "Scott8MCTargetDesc.h"
#include "Scott8InstPrinter.h"
#include "Scott8MCAsmInfo.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "Scott8GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Scott8GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Scott8GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createScott8MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitScott8MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createScott8MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitScott8MCRegisterInfo(X, Scott8::PC);
  return X;
}

static MCAsmInfo *createScott8MCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  return new Scott8MCAsmInfo();
}

static MCSubtargetInfo *createScott8MCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createScott8MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/CPU, FS);
}

static MCInstPrinter *createScott8MCInstPrinter(const Triple &TT, unsigned SyntaxVariant,
                       const MCAsmInfo &MAI, const MCInstrInfo &MII,
                       const MCRegisterInfo &MRI) {
  return new Scott8InstPrinter(MAI, MII, MRI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeScott8TargetMC() {
  RegisterMCAsmInfoFn X(getTheScott8Target(), createScott8MCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(getTheScott8Target(), createScott8MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(getTheScott8Target(), createScott8MCRegisterInfo);
  TargetRegistry::RegisterMCAsmBackend(getTheScott8Target(), createScott8AsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(getTheScott8Target(), createScott8MCCodeEmitter);
  TargetRegistry::RegisterMCSubtargetInfo(getTheScott8Target(), createScott8MCSubtargetInfo);
  TargetRegistry::RegisterMCInstPrinter(getTheScott8Target(), createScott8MCInstPrinter);
}
