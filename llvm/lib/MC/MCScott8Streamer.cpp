//===- lib/MC/MCScott8Streamer.cpp - Scott Emulator Object Output ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file assembles .s files and emits Scott8 .o object files.
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCScott8Streamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmLayout.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectStreamer.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCSectionScott8.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSymbolScott8.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

MCScott8Streamer::~MCScott8Streamer() = default;

void MCScott8Streamer::emitInstToData(const MCInst &Inst, const MCSubtargetInfo &STI) {
  MCDataFragment *DF = getOrCreateDataFragment();
  SmallVector<MCFixup, 4> Fixups;
  SmallString<256> Code;
  raw_svector_ostream VecOS(Code);
  getAssembler().getEmitter().encodeInstruction(Inst, VecOS, Fixups, STI);

  // Add the fixups and data.
  for (MCFixup &Fixup : Fixups) {
    Fixup.setOffset(Fixup.getOffset() + DF->getContents().size());
    DF->getFixups().push_back(Fixup);
  }
  DF->setHasInstructions(STI);
  DF->getContents().append(Code.begin(), Code.end());
}

bool MCScott8Streamer::emitSymbolAttribute(MCSymbol *S, MCSymbolAttr Attribute) {
  return true;
}

void MCScott8Streamer::emitCommonSymbol(MCSymbol *S, uint64_t Size, Align ByteAlignment) {
  llvm_unreachable("Not implemented.");
}

void MCScott8Streamer::emitZerofill(MCSection *Section, MCSymbol *Symbol,
                                  uint64_t Size, Align ByteAlignment,
                                  SMLoc Loc) {
  llvm_unreachable("Not implemented.");
}

void MCScott8Streamer::emitIntValue(uint64_t Value, unsigned Size) {
  MCObjectStreamer::emitIntValue(Value, Size);
}

void MCScott8Streamer::emitFill(const MCExpr &NumBytes, uint64_t FillValue, SMLoc Loc) {
  MCObjectStreamer::emitFill(NumBytes, FillValue, Loc);
}

MCStreamer *llvm::createScott8Streamer(MCContext &Context,
                                     std::unique_ptr<MCAsmBackend> &&MAB,
                                     std::unique_ptr<MCObjectWriter> &&OW,
                                     std::unique_ptr<MCCodeEmitter> &&CE,
                                     bool RelaxAll) {
  MCScott8Streamer *S = new MCScott8Streamer(Context, std::move(MAB), std::move(OW), std::move(CE));
  if (RelaxAll)
    S->getAssembler().setRelaxAll(true);
  return S;
}

