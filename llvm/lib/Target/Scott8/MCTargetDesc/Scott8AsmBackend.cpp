//===-- Scott8AsmBackend.cpp - Scott8 Assembler Backend ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Scott8MCTargetDesc.h"
#include "MCTargetDesc/Scott8FixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCScott8ObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

namespace {
class Scott8AsmBackend : public MCAsmBackend {

public:
  Scott8AsmBackend() : MCAsmBackend(support::little) {}

  ~Scott8AsmBackend() {}

  bool writeNopData(raw_ostream &OS, uint64_t Count, const MCSubtargetInfo *STI) const override;

  unsigned getNumFixupKinds() const override {
    return Scott8::NumTargetFixupKinds;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[Scott8::NumTargetFixupKinds] = {
      // This table *must* be in the order that the fixup_* kinds are defined in Scott8FixupKinds.h.
      // Name            Offset (bits)     Size (bits)     Flags
      { "Scott8FixupNone",     0,  8,   0 },
      { "Scott8OrdinaryFixup", 0,  8,   0 },
    };

    if (Kind < FirstTargetFixupKind) {
      return MCAsmBackend::getFixupKindInfo(Kind);
    }

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() && "Invalid kind!");

    return Infos[Kind - FirstTargetFixupKind];
  }

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override;

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }
};

bool Scott8AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count, const MCSubtargetInfo *STI) const {
  // We can't write nops.
  return !Count;
}

std::unique_ptr<MCObjectTargetWriter> Scott8AsmBackend::createObjectTargetWriter() const {
  return std::make_unique<MCScott8ObjectTargetWriter>();
}

} // end anonymous namespace

void Scott8AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const {
  if (!Value) {
    return;
  }

  // Address offset to make adjustments to.
  unsigned Offset = Fixup.getOffset();
  Data[Offset] = Value & 0xFF;
  
  return;
}

MCAsmBackend *llvm::createScott8AsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  return new Scott8AsmBackend();
}
