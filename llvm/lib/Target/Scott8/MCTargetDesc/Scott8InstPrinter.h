//===-- Scott8InstPrinter.h - Convert Scott8 MCInst to assembly syntax ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an Scott8 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8INSTPRINTER_H
#define LLVM_LIB_TARGET_Scott8_MCTARGETDESC_Scott8INSTPRINTER_H

#include "llvm/MC/MCInstPrinter.h"

namespace llvm {

class TargetMachine;

class Scott8InstPrinter : public MCInstPrinter {
public:
  Scott8InstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                    const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  void printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                 const MCSubtargetInfo &STI, raw_ostream &O) override;

  virtual void printRegName(raw_ostream &OS, unsigned RegNo) const override;

  // Autogenerated by tblgen.
  std::pair<const char *, uint64_t> getMnemonic(const MCInst *MI) override;
  void printInstruction(const MCInst *MI, uint64_t Address, const MCSubtargetInfo &STI, raw_ostream &O);
  bool printAliasInstr(const MCInst *MI, uint64_t Address, const MCSubtargetInfo &STI, raw_ostream &O);
  void printOperand(const MCInst *MI, unsigned OpNo, const MCSubtargetInfo &STI, raw_ostream &O);
  void printCCOperand(const MCInst *MI, unsigned OpNo, const MCSubtargetInfo &STI, raw_ostream &O);
  static const char *getRegisterName(unsigned RegNo);
};
} // end namespace llvm

#endif
