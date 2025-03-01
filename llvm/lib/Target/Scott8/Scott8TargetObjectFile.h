//===-- Scott8TargetObjectFile.h - Scott8 Object Info -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_Scott8TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_Scott8_Scott8TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

class MCContext;
class TargetMachine;

class Scott8TargetObjectFile : public TargetLoweringObjectFileScott8 {
public:
  Scott8TargetObjectFile() : TargetLoweringObjectFileScott8() {}

  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif
