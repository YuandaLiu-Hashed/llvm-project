//===- lib/MC/MCSectionScott8.cpp - Scott8 Code Section Representation --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/MC/MCSectionScott8.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSymbolScott8.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

MCSectionScott8::MCSectionScott8(StringRef Name, SectionKind K, MCSymbol *Begin)
    : MCSection(SV_Scott8, Name, K, Begin) {}

MCSectionScott8::~MCSectionScott8() {} // anchor.

void MCSectionScott8::printSwitchToSection(const MCAsmInfo &MAI, const Triple &T,
                                           raw_ostream &OS,
                                           const MCExpr *Subsection) const { }
