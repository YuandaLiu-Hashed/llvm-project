//===-- llvm/MC/MCScott8ObjectWriter.h - Scott8 Object Writer -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCSCOTT8OBJECTWRITER_H
#define LLVM_MC_MCSCOTT8OBJECTWRITER_H

#include "llvm/MC/MCObjectWriter.h"
#include <memory>

namespace llvm {

class MCFixup;
class MCValue;
class raw_pwrite_stream;

class MCScott8ObjectTargetWriter : public MCObjectTargetWriter {
public:
  explicit MCScott8ObjectTargetWriter();

  virtual ~MCScott8ObjectTargetWriter();

  Triple::ObjectFormatType getFormat() const override { return Triple::Scott8; }
  static bool classof(const MCObjectTargetWriter *W) {
    return W->getFormat() == Triple::Scott8;
  }
};

std::unique_ptr<MCObjectWriter> createScott8ObjectWriter(std::unique_ptr<MCScott8ObjectTargetWriter> MOTW, raw_pwrite_stream &OS);

} // namespace llvm

#endif // LLVM_MC_MCSCOTT8OBJECTWRITER_H
