//===-- Scott8AsmPrinter.cpp - Scott8 LLVM assembly writer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format Scott8 assembly language.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Scott8InstPrinter.h"
#include "Scott8.h"
#include "Scott8InstrInfo.h"
#include "Scott8MCInstLower.h"
#include "Scott8TargetMachine.h"
#include "TargetInfo/Scott8TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {
  class Scott8AsmPrinter : public AsmPrinter {
  
  Scott8MCInstLower MCInstLowering;

  public:
    explicit Scott8AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
        : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(OutContext, *this) {
    }

    StringRef getPassName() const override {
        return StringRef("Scott8 Assembly Printer");
    }

    void emitFunctionEntryLabel() override;
    void emitInstruction(const MachineInstr *MI) override;
    void emitFunctionBodyStart() override;
    void expandCallInstruction(const MachineInstr *MI);
    void expandRetInstruction();
  };
} // end of anonymous namespace

void Scott8AsmPrinter::emitFunctionBodyStart() {}

void Scott8AsmPrinter::emitFunctionEntryLabel() {
  OutStreamer->emitLabel(CurrentFnSym);
}

void Scott8AsmPrinter::expandCallInstruction(const MachineInstr *MI) {
  MCSymbol *RetSymbol = OutContext.createTempSymbol();
  const MCExpr *RetExpr = MCSymbolRefExpr::create(RetSymbol, OutContext);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);

  // Address that SP points to will contain the return address.
  EmitToStreamer(*OutStreamer, MCInstBuilder(Scott8::CPYri).addReg(Scott8::R2).addExpr(RetExpr));
  EmitToStreamer(*OutStreamer, MCInstBuilder(Scott8::ST).addReg(Scott8::R2).addReg(Scott8::SP));
  EmitToStreamer(*OutStreamer, MCInstBuilder(Scott8::JMP).addOperand(TmpInst.getOperand(0)));
  OutStreamer->emitLabel(RetSymbol);
}

void Scott8AsmPrinter::expandRetInstruction() {
  // Load return address to the temp reg and jump to it.
  EmitToStreamer(*OutStreamer, MCInstBuilder(Scott8::LD).addReg(Scott8::R2).addReg(Scott8::SP));
  EmitToStreamer(*OutStreamer, MCInstBuilder(Scott8::JMPrr).addReg(Scott8::R2));
}

void Scott8AsmPrinter::emitInstruction(const MachineInstr *MI) {
  MachineBasicBlock::const_instr_iterator I = MI->getIterator();
  MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();

  do {
    switch (I->getOpcode())
    {
      case Scott8::CALL_PSEUDO:
        expandCallInstruction(&*I);
        return;
      case Scott8::RET_PSEUDO:
        expandRetInstruction();
        return;
    }

    MCInst TmpInst;
    MCInstLowering.Lower(&*I, TmpInst);
    EmitToStreamer(*OutStreamer, TmpInst);

  } while ((++I != E) && I->isInsideBundle());
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeScott8AsmPrinter() {
  RegisterAsmPrinter<Scott8AsmPrinter> X(getTheScott8Target());
}
