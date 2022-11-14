//===-- Scott8.h - Top-level interface for Scott8 representation --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// Scott8 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_Scott8_H
#define LLVM_LIB_TARGET_Scott8_Scott8_H

#include "MCTargetDesc/Scott8MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace Scott8CC {
  enum CondCodes {
    COND_E      = 0b0010,    // equals
    COND_C      = 0b1000,    // carry
    COND_A      = 0b0100,    // greater
    COND_Z      = 0b0001,    // zero
    COND_AE     = 0b0110,    // greater or equal
    COND_INVALID = -1
  };
}

namespace llvm {
  class Scott8TargetMachine;
  class FunctionPass;

  FunctionPass *createScott8ISelDag(Scott8TargetMachine &TM);
} // end namespace llvm;

#endif
