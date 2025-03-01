//===-- MCScott8ObjectTargetWriter.cpp - Scott8 Target Writer Subclass --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCScott8ObjectWriter.h"

using namespace llvm;

MCScott8ObjectTargetWriter::MCScott8ObjectTargetWriter() {}

// Pin the vtable to this object file
MCScott8ObjectTargetWriter::~MCScott8ObjectTargetWriter() = default;
