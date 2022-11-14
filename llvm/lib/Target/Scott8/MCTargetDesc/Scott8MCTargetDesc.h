//===-- Scott8MCTargetDesc.h - Scott8 Target Descriptions ---------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8MCTARGETDESC_H
#define LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8MCTARGETDESC_H

#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class Target;
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;

class StringRef;
class raw_ostream;
class raw_pwrite_stream;
class Triple;

Target &getTheScott8Target();

MCCodeEmitter *createScott8MCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);

MCAsmBackend *createScott8AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options);

} // End llvm namespace

// Defines symbolic names for Scott8 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "Scott8GenRegisterInfo.inc"

// Defines symbolic names for the Scott8 instructions.
//
#define GET_INSTRINFO_ENUM
#include "Scott8GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "Scott8GenSubtargetInfo.inc"

#endif
