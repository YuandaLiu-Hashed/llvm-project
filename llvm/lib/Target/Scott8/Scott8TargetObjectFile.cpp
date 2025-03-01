//===------- Scott8TargetObjectFile.cpp - Scott8 Object Info Impl -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Scott8TargetObjectFile.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

void Scott8TargetObjectFile::Initialize(MCContext &Ctx,
                                        const TargetMachine &TM) {
  TargetLoweringObjectFileScott8::Initialize(Ctx, TM);
}
