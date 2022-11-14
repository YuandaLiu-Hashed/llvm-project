//===-- Scott8Subtarget.cpp - Scott8 Subtarget Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the Scott8 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "Scott8Subtarget.h"
#include "Scott8.h"

#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "Scott8-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Scott8GenSubtargetInfo.inc"

void Scott8Subtarget::anchor() { }

Scott8Subtarget::Scott8Subtarget(const Triple &TT, const std::string &CPU,
                               const std::string &FS, const TargetMachine &TM)
    : Scott8GenSubtargetInfo(TT, CPU, /*TUNE CPU*/CPU, FS),
      InstrInfo(*this), TLInfo(TM, *this),
      FrameLowering(*this) {}
