//===- lib/MC/Scott8ObjectWriter.cpp - Scott8 File Writer ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements Scott8 object file writer information.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/BinaryFormat/Scott8.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionScott8.h"
#include "llvm/MC/MCSymbolScott8.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/MCScott8ObjectWriter.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/LEB128.h"
#include "llvm/Support/StringSaver.h"
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "mc"

namespace {

struct Scott8RelocationEntry {
  const uint64_t Offset;
  const uint64_t FixedValue;
  const MCSection *FixupSection;

  Scott8RelocationEntry(uint64_t Offset, int64_t FixedValue, const MCSection *FixupSection)
      : Offset(Offset), FixedValue(FixedValue),
        FixupSection(FixupSection) {}
};

class Scott8ObjectWriter : public MCObjectWriter {
  support::endian::Writer W;
  std::unique_ptr<MCScott8ObjectTargetWriter> TargetObjectWriter;
  std::vector<Scott8RelocationEntry> Relocations;
public:
  Scott8ObjectWriter(std::unique_ptr<MCScott8ObjectTargetWriter> MOTW, raw_pwrite_stream &OS)
      : W(OS, support::little), TargetObjectWriter(std::move(MOTW)) {}


  void recordRelocation(MCAssembler &Asm, const MCAsmLayout &Layout,
                        const MCFragment *Fragment, const MCFixup &Fixup,
                        MCValue Target, uint64_t &FixedValue) override;

  void executePostLayoutBinding(MCAssembler &Asm, const MCAsmLayout &Layout) override;

  uint64_t writeObject(MCAssembler &Asm, const MCAsmLayout &Layout) override;
};

void Scott8ObjectWriter::recordRelocation(MCAssembler &Asm,
                                        const MCAsmLayout &Layout,
                                        const MCFragment *Fragment,
                                        const MCFixup &Fixup, MCValue Target,
                                        uint64_t &FixedValue) {

  uint64_t FixupOffset = Layout.getFragmentOffset(Fragment) + Fixup.getOffset();
  uint64_t ValueToRecord = FixedValue;

  Scott8RelocationEntry RelocationEntry(FixupOffset, ValueToRecord, Fragment->getParent());

  Relocations.push_back(RelocationEntry);
}

void Scott8ObjectWriter::executePostLayoutBinding(MCAssembler &Asm, const MCAsmLayout &Layout) {}

static void patchI8(raw_pwrite_stream &Stream, uint8_t X, uint64_t Offset) {
  uint8_t Buffer[1];
  Buffer[0] = X;
  Stream.pwrite((char *)Buffer, 1, Offset);
}

uint64_t Scott8ObjectWriter::writeObject(MCAssembler &Asm, const MCAsmLayout &Layout) {
  uint64_t StartOffset = W.OS.tell();

  // Write the section.
  assert(Asm.size() == 1 && "Single section is expected");
  const MCSection &Section = *Asm.begin();
  Asm.writeSectionData(W.OS, &Section, Layout);

  // Patch fixups according to scott8 rules.
  raw_pwrite_stream &Stream = static_cast<raw_pwrite_stream&>(W.OS);

  for (const Scott8RelocationEntry &Relocation : Relocations) {
    uint8_t value = scott8::calculateAddress(Relocation.FixedValue);
    patchI8(Stream, value, Relocation.Offset);
  }

  return W.OS.tell() - StartOffset;
}

} // end anonymous namespace

std::unique_ptr<MCObjectWriter> llvm::createScott8ObjectWriter(
    std::unique_ptr<MCScott8ObjectTargetWriter> MOTW, raw_pwrite_stream &OS) {
  return std::make_unique<Scott8ObjectWriter>(std::move(MOTW), OS);
}

