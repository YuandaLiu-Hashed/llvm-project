//===--- Scott8.h - Scott8 ToolChain Implementations --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_Scott8_H
#define LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_Scott8_H

#include "clang/Driver/ToolChain.h"

namespace clang {
namespace driver {
namespace toolchains {

class LLVM_LIBRARY_VISIBILITY Scott8ToolChain : public ToolChain {
public:
  Scott8ToolChain(const Driver &D, const llvm::Triple &Triple,
                 const llvm::opt::ArgList &Args)
      : ToolChain(D, Triple, Args) {}

  bool isPICDefault() const override { return false; }
  bool isPIEDefault(const llvm::opt::ArgList &Args) const override { return false; }
  bool isPICDefaultForced() const override { return false; }
  bool IsIntegratedAssemblerDefault() const override { return true; }
};

} // end namespace toolchains
} // end namespace driver
} // end namespace clang

#endif // LLVM_CLANG_LIB_DRIVER_TOOLCHAINS_Scott8_H
