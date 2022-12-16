//===-- Scott8TargetMachine.cpp - Define TargetMachine for Scott8 -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "Scott8TargetMachine.h"
#include "Scott8.h"
#include "Scott8TargetObjectFile.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeScott8Target() {
  // Register the target.
  RegisterTargetMachine<Scott8TargetMachine> X(getTheScott8Target());
}

Scott8TargetMachine::Scott8TargetMachine(
    const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
    const TargetOptions &Options, std::optional<Reloc::Model> RM,
    std::optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, "e-p:8:8:8-i8:8:8-n8-a:0:8", TT, CPU, FS, Options,
                        Reloc::Static, CodeModel::Small, OL),
      TLOF(std::make_unique<Scott8TargetObjectFile>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();
}

Scott8TargetMachine::~Scott8TargetMachine() {}

const Scott8Subtarget *Scott8TargetMachine::getSubtargetImpl(const Function &F) const {
  return &Subtarget;
}

namespace {
/// Scott8 Code Generator Pass Configuration Options.
class Scott8PassConfig : public TargetPassConfig {
public:
  Scott8PassConfig(Scott8TargetMachine &TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  Scott8TargetMachine &getScott8TargetMachine() const {
    return getTM<Scott8TargetMachine>();
  }

  bool addInstSelector() override;
  void addPreRegAlloc() override;
  void addPostRegAlloc() override;
  void addPreEmitPass() override;
};
} // namespace

namespace llvm {
  FunctionPass *createBundleCmpJccPass();
  FunctionPass *createInsertClfPass();
  FunctionPass *createInsertInitialStackStatePass();
  FunctionPass *createExpandPostRAPseudoPass();
}

TargetPassConfig *Scott8TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Scott8PassConfig(*this, PM);
}

bool Scott8PassConfig::addInstSelector() {
  addPass(llvm::createScott8ISelDag(getScott8TargetMachine()));
  return false;
}

void Scott8PassConfig::addPreRegAlloc() {
  addPass(llvm::createBundleCmpJccPass());
}

void Scott8PassConfig::addPostRegAlloc() {
  // addPass(llvm::createExpandPostRAPseudoPass());
}

void Scott8PassConfig::addPreEmitPass() {
  addPass(llvm::createInsertClfPass());
  addPass(llvm::createInsertInitialStackStatePass());
  addPass(llvm::createExpandPostRAPseudoPass());
}
