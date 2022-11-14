//===-- Scott8TargetMachine.h - Define TargetMachine for Scott8 ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the Scott8 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_Scott8TARGETMACHINE_H
#define LLVM_LIB_TARGET_Scott8_Scott8TARGETMACHINE_H

#include "Scott8InstrInfo.h"
#include "Scott8Subtarget.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class Scott8TargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  Scott8Subtarget Subtarget;
public:
  Scott8TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT);
  ~Scott8TargetMachine() override;

  const Scott8Subtarget *getSubtargetImpl(const Function &) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
};

} // end namespace llvm

#endif
