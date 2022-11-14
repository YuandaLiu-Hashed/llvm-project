//===-- Scott8InstrFormats.td - Scott8 Instruction Formats ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Regular 8-bit instruction
class InstScott8<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<8> Inst;

  let Namespace = "Scott8";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  let Size = 1;
}

//Some instructions like DATA or JUMP need double space (16 bit).
//It is encoded as 16 bit.
class InstScott8Double<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
  field bits<16> Inst;

  let Namespace = "Scott8";
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
  let Size = 2;
}

// Pseudo instruction like call/ret
class Scott8PseudoInst<dag outs, dag ins, string asmstr, list<dag> pattern> 
    : InstScott8<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

// ALU register-register instruction
class InstScott8Alu<bits<4> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstScott8<outs, ins, asmstr, pattern> {
  bits<2> destReg;
  bits<2> sourceReg;
  //76543210
  //1CCCSSDD
  let Inst{1-0} = destReg;
  let Inst{3-2} = sourceReg;
  let Inst{7-4} = opcode;
}