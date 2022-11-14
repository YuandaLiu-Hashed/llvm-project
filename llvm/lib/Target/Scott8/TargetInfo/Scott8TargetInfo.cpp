//===-- Scott8TargetInfo.cpp - Scott8 Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheScott8Target() {
  static Target TheScott8Target;
  return TheScott8Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeScott8TargetInfo() {
  RegisterTarget<Triple::scott8, /*HasJIT=*/false> X(getTheScott8Target(), "Scott8",
                                                   "Scott8 CPU", "Scott8");
}