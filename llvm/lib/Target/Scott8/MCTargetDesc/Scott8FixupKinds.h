//===-- Scott8FixupKinds.h - Scott8 Specific Fixup Entries ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_FIXUPKINDS_H
#define LLVM_LIB_TARGET_Scott8_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Scott8 {

  // This table *must* be in the same order of
  // MCFixupKindInfo Infos[Scott8::NumTargetFixupKinds]
  // in Scott8AsmBackend.cpp.
  enum Fixups {
    Scott8FixupNone = FirstTargetFixupKind,
    Scott8OrdinaryFixup,
    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
} // namespace Scott8
} // namespace llvm


#endif
