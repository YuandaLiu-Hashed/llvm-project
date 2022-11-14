//===- Scott8MCAsmInfo.h - Scott8 asm properties -----------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Scott8MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8MCASMINFO_H
#define LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class Scott8MCAsmInfo : public MCAsmInfo {
  virtual void anchor();

public:
  explicit Scott8MCAsmInfo();
};

}

#endif // LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8MCASMINFO_H
