//===--- Scott8.h - Declare Scott8 target feature support -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares Scott8 TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_SCOTT8_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_SCOTT8_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY Scott8TargetInfo : public TargetInfo {
public:
  Scott8TargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    NoAsmVariants = true;
    IntWidth = 8;
    LongWidth = 8;
    BoolWidth = 8;
    LongLongWidth = 8;
    IntAlign = LongAlign = BoolAlign = LongLongAlign = 8;
    SuitableAlign = 8;
    DoubleAlign = LongDoubleAlign = 8;
    PointerWidth = PointerAlign = 8;
    SizeType = UnsignedShort;
    PtrDiffType = SignedShort;
    IntPtrType = SignedShort;
    UseZeroLengthBitfieldAlignment = true;
    resetDataLayout("e-p:8:8:8-i8:8:8-n8-a:0:8");
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  ArrayRef<Builtin::Info> getTargetBuiltins() const override { return std::nullopt; }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::VoidPtrBuiltinVaList;
  }

  const char *getClobbers() const override { return ""; }

  ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {
        "R0",  "R1",  "R2",  "R3"};
    return llvm::makeArrayRef(GCCRegNames);
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return std::nullopt;
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    return false;
  }
};

} // namespace targets
} // namespace clang

#endif // LLVM_CLANG_LIB_BASIC_TARGETS_SCOTT8_H
