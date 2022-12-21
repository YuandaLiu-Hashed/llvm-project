//===-- Scott8InstrInfo.td - Target Description for Scott8 Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the Scott8 instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "Scott8InstrFormats.td"

//===----------------------------------------------------------------------===//
// Type Profiles.
//===----------------------------------------------------------------------===//

def SDT_Scott8Cmp          : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def SDT_Scott8BrCC         : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>, SDTCisVT<1, i8>]>;
def SDT_Scott8SelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>, SDTCisSameAs<1, 2>, SDTCisVT<3, i8>]>;
def SDT_Scott8Wrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>, SDTCisPtrTy<0>]>;
def SDT_Scott8Call         : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_Scott8CallSeqStart : SDCallSeqStart<[SDTCisVT<0, i8>, SDTCisVT<1, i8>]>;
def SDT_Scott8CallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i8>, SDTCisVT<1, i8>]>;
def SDT_Scott8IoIn         : SDTypeProfile<1, 0, [SDTCisVT<0, i8>]>;
def SDT_Scott8IoOut        : SDTypeProfile<0, 1, [SDTCisVT<0, i8>]>;
def SDT_Clf                : SDTypeProfile<0, 0, []>;

//===----------------------------------------------------------------------===//
// Scott8 Specific Node Definitions.
//===----------------------------------------------------------------------===//

def Scott8Callseq_start    : SDNode<"ISD::CALLSEQ_START", SDT_Scott8CallSeqStart, [SDNPHasChain, SDNPOutGlue]>;
def Scott8Callseq_end      : SDNode<"ISD::CALLSEQ_END",   SDT_Scott8CallSeqEnd, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def Scott8RetFlag          : SDNode<"Scott8ISD::RET_FLAG", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def Scott8Cmp              : SDNode<"Scott8ISD::CMP", SDT_Scott8Cmp, [SDNPOutGlue, SDNPInGlue]>;
def Scott8Brcc             : SDNode<"Scott8ISD::BR_CC", SDT_Scott8BrCC, [SDNPHasChain, SDNPInGlue]>;
def Scott8SelectCC         : SDNode<"Scott8ISD::SELECT_CC", SDT_Scott8SelectCC, [SDNPInGlue]>;
def Scott8Wrapper          : SDNode<"Scott8ISD::Wrapper", SDT_Scott8Wrapper>;
def Scott8Call             : SDNode<"Scott8ISD::CALL", SDT_Scott8Call, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def Scott8IoDataIn         : SDNode<"Scott8ISD::IO_DATA_IN", SDT_Scott8IoIn, [SDNPHasChain]>;
def Scott8IoDataOut        : SDNode<"Scott8ISD::IO_DATA_OUT", SDT_Scott8IoOut, [SDNPHasChain]>;
def Scott8IoAddressOut     : SDNode<"Scott8ISD::IO_ADDRESS_OUT", SDT_Scott8IoOut, [SDNPHasChain]>;
def Scott8Clf              : SDNode<"Scott8ISD::CLF", SDT_Clf, [SDNPOutGlue]>;
def Scott8NoClfAdd         : SDNode<"Scott8ISD::NO_CLF_ADD", SDTIntBinOp, [SDNPCommutative, SDNPAssociative]>;

//===----------------------------------------------------------------------===//
// Scott8 Operand Definitions.
//===----------------------------------------------------------------------===//

// Immediate/Constant operand
def Immediate8 : Operand<i8>, ImmLeaf<i8, [{
  return Imm >= -128 && Imm < 128;
}]>;

def JmpTarget : Operand<OtherVT> {
}

// Operand for printing out a condition code.
def ConditionCode : Operand<i8> {
  let PrintMethod = "printCCOperand";
}

//===----------------------------------------------------------------------===//
// Scott8 Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 1, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// ALU Instructions
//===----------------------------------------------------------------------===//

let Defs = [FR], isCommutable = 1, Constraints = "$sourceReg2 = $destReg" in {
  multiclass ALU<bits<4> opcode, string opstr, SDNode opnode> {
    def rr : InstScott8Alu<opcode,
                           (outs GPRegs:$destReg), (ins GPRegs:$sourceReg2, GPRegs:$sourceReg),
                           !strconcat(opstr, "\t$sourceReg, $destReg"),
                           [(set GPRegs:$destReg, (opnode GPRegs:$sourceReg2, GPRegs:$sourceReg)),
                           (implicit FR)]> {
    }
  }

  defm ADD        : ALU<0b1000, "ADD", add>;
  defm NO_CLF_ADD : ALU<0b1000, "ADD", Scott8NoClfAdd>;
  defm AND        : ALU<0b1100, "AND", and>;
  defm OR         : ALU<0b1101, "OR",  or>;
  defm XOR        : ALU<0b1110, "XOR", xor>;

  def NOTrr : InstScott8<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg2), 
              "NOT\t$sourceReg2", 
              [(set GPRegs:$destReg, (not GPRegs:$sourceReg2)), (implicit FR)]> {
    bits<2> destReg;
    bits<2> sourceReg2;
    let Inst{1-0} = destReg;
    let Inst{3-2} = sourceReg2;
    let Inst{7-4} = 0b1011;
  }
}

//===----------------------------------------------------------------------===//
// Shift Instructions
//===----------------------------------------------------------------------===//

let Defs = [FR] /*Constraints = "$sourceReg = $destReg"*/ in {
  def SHL : InstScott8<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg), 
            "SHL\t$sourceReg, $destReg", 
            [(set GPRegs:$destReg, (shl GPRegs:$sourceReg, 1)), (implicit FR)]> {
    bits<2> destReg;
    bits<2> sourceReg;
    let Inst{1-0} = destReg;
    let Inst{3-2} = sourceReg;
    let Inst{7-4} = 0b1010;
  }

  def SHR : InstScott8<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg), 
            "SHR\t$sourceReg, $destReg", 
            [(set GPRegs:$destReg, (srl GPRegs:$sourceReg, 1)), (implicit FR)]> {
    bits<2> destReg;
    bits<2> sourceReg;
    let Inst{1-0} = destReg;
    let Inst{3-2} = sourceReg;
    let Inst{7-4} = 0b1001;
  }
}

//===----------------------------------------------------------------------===//
// Data Transfer Instructions
//===----------------------------------------------------------------------===//

let isAsCheapAsAMove = 1 in {
  def CPYri : InstScott8Double<(outs GPRegs:$destReg), (ins Immediate8:$const),
              "DATA\t$destReg, $const",
              [(set GPRegs:$destReg, Immediate8:$const)]> {
    bits<2> destReg;
    bits<8> const;
    let Inst{5} = 0b1;
    let Inst{1-0} = destReg;
    let Inst{15-8} = const;
  }
} // isAsCheapAsAMove = 1

//===----------------------------------------------------------------------===//
// Comparison Instructions
//===----------------------------------------------------------------------===//

let Defs = [FR] in {
  def CMPrr : InstScott8<(outs), (ins GPRegs:$reg1, GPRegs:$reg2), 
              "CMP\t$reg1, $reg2", 
              [(Scott8Cmp GPRegs:$reg1, GPRegs:$reg2), (implicit FR)]> {
    bits<2> reg1;
    bits<2> reg2;
    let Inst{1-0} = reg2;
    let Inst{3-2} = reg1;
    let Inst{7-4} = 0b1111;
  }
}

let hasSideEffects = 1, Defs = [FR] in {
  def CLF : InstScott8<(outs), (ins), "CLF", [(Scott8Clf), (implicit FR)]> {
    let Inst{6-5} = 0b11;
  }
}

//===----------------------------------------------------------------------===//
// Load/Store Instructions
//===----------------------------------------------------------------------===//

def LD : InstScott8<(outs GPRegs:$destReg), (ins GPRegs:$addr), 
         "LOAD\t$addr, $destReg", 
         [(set GPRegs:$destReg, (load addr:$addr))]> {
  bits<2> destReg;
  bits<8> addr;
  let Inst{3-2} = addr{1-0};
  let Inst{1-0} = destReg;
}

def ST : InstScott8<(outs), (ins GPRegs:$regToStore, GPRegs:$addr), 
         "STORE\t$addr, $regToStore", 
         [(store GPRegs:$regToStore, addr:$addr)]> {
  bits<2> regToStore;
  bits<8> addr;
  let Inst{4} = 0b1;
  let Inst{3-2} = addr{1-0};
  let Inst{1-0} = regToStore;
}

//===----------------------------------------------------------------------===//
// Jump (branch) Instructions
//===----------------------------------------------------------------------===//

// Unconditional jump
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
  def JMP : InstScott8Double<(outs), (ins JmpTarget:$addr), 
            "JMP\t$addr", 
            [(br bb:$addr)]> {
    bits<8> addr;
    //76543210
    //01000000
    let Inst{6} = 0b1;
    let Inst{15-8} = addr;
  }

  let isIndirectBranch = 1 in {
    def JMPrr : InstScott8<(outs), (ins GPRegs:$targetReg), 
                "JMPR\t$targetReg", 
                [(brind GPRegs:$targetReg)]> {
      bits<2> targetReg;
      //76543210
      //011000XX
      let Inst{5-4} = 0b11;
      let Inst{1-0} = targetReg;
    }
  }
}

// Conditional jump
let isBranch = 1, isTerminator = 1, Uses=[FR] in {
  def JCC : InstScott8Double<(outs), (ins JmpTarget:$addr, ConditionCode:$condition),
            "J$condition\t$addr", 
            [(Scott8Brcc bb:$addr, imm:$condition)]> {
    bits<8> addr;
    bits<4> condition;
    //76543210
    //0101CCCC
    //AAAAAAAA
    let Inst{3-0} = condition;
    let Inst{6-4} = 0b101;
    let Inst{15-8} = addr;
  }
}

//===----------------------------------------------------------------------===//
// Pseudo Instructions
//===----------------------------------------------------------------------===//

let Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN : Scott8PseudoInst<(outs), (ins Immediate8:$amt, Immediate8:$amt2),
                                           "#ADJCALLSTACKDOWN",
                                           [(Scott8Callseq_start timm:$amt, timm:$amt2)]>;
  def ADJCALLSTACKUP : Scott8PseudoInst<(outs), (ins Immediate8:$amt1, Immediate8:$amt2),
                                        "#ADJCALLSTACKUP",
                                        [(Scott8Callseq_end timm:$amt1, timm:$amt2)]>;
}

let Uses = [SP], Defs = [R1], isCall = 1 in {
  def CALL_PSEUDO : Scott8PseudoInst<(outs), (ins Immediate8:$addr),
                      "# call\t$addr", [(Scott8Call imm:$addr)]> {
  }
}

let Uses = [SP], isTerminator = 1, isReturn = 1, isBarrier = 1 in {
  def RET_PSEUDO : Scott8PseudoInst<(outs), (ins),
                    "# ret",  [(Scott8RetFlag)]> {
  }
}

//===----------------------------------------------------------------------===//
// Arithmatics Psudos
//===----------------------------------------------------------------------===//

let Constraints = "$sourceReg2 = $destReg,@earlyclobber $destReg", Defs = [FR] in {
  def SUB_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg2, GPRegs:$sourceReg),
                   "# SUB\t$sourceReg, $destReg",
                   [(set GPRegs:$destReg, (sub GPRegs:$sourceReg2, GPRegs:$sourceReg)), (implicit FR)]> {
  }

  def UMUL_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg2, GPRegs:$sourceReg),
                    "# UMUL\t$sourceReg, $destReg",
                    [(set GPRegs:$destReg, (mul GPRegs:$sourceReg2, GPRegs:$sourceReg)), (implicit FR), (set GPRegs:$sourceReg)]> {
  }
}

//===----------------------------------------------------------------------===//
// Logical Shift Psudos
//===----------------------------------------------------------------------===//

let Defs = [FR] in {
  def SHLri_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg, Immediate8:$amount),
                       "# SHLri # shift $sourceReg by $amount to the left.", 
                       [(set GPRegs:$destReg, (shl GPRegs:$sourceReg, imm:$amount)), (implicit FR)]> {
  }

  def SHRri_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg, Immediate8:$amount),
                       "# SHRri # shift $sourceReg by $amount to the right.", 
                       [(set GPRegs:$destReg, (srl GPRegs:$sourceReg, imm:$amount)), (implicit FR)]> {
  }
}

let Constraints = "$sourceReg2 = $destReg,@earlyclobber $destReg", Defs = [FR] in {
  def SHLrr_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg2, GPRegs:$sourceReg),
                       "# SHLrr # shift $destReg by $sourceReg amount to the left.", 
                       [(set GPRegs:$destReg, (shl GPRegs:$sourceReg2, GPRegs:$sourceReg)), (implicit FR), (set GPRegs:$sourceReg)]> {
  }

  def SHRrr_PSEUDO : Scott8PseudoInst<(outs GPRegs:$destReg), (ins GPRegs:$sourceReg2, GPRegs:$sourceReg),
                       "# SHRrr # shift $destReg by $sourceReg amount to the right.", 
                       [(set GPRegs:$destReg, (srl GPRegs:$sourceReg2, GPRegs:$sourceReg)), (implicit FR), (set GPRegs:$sourceReg)]> {
  }
}

//===----------------------------------------------------------------------===//
// I/O Instructions
//===----------------------------------------------------------------------===//

let hasSideEffects = 1 in {
  def IO_DATA_IN : InstScott8<(outs GPRegs:$destReg), (ins), 
                   "IN\tData, $destReg", 
                   [(set GPRegs:$destReg, Scott8IoDataIn)]> {
    bits<2> destReg;
    let Inst{6-4} = 0b111;
    let Inst{1-0} = destReg;
  }

  def IO_DATA_OUT : InstScott8<(outs), (ins GPRegs:$regToOut), 
                    "OUT\tData, $regToOut", 
                    [(Scott8IoDataOut GPRegs:$regToOut)]> {
    bits<2> regToOut;
    let Inst{6-3} = 0b1111;
    let Inst{1-0} = regToOut;
  }

  def IO_ADDRESS_OUT : InstScott8<(outs), (ins GPRegs:$regToOut), 
                       "OUT\tAddr, $regToOut", 
                       [(Scott8IoAddressOut GPRegs:$regToOut)]> {
    bits<2> regToOut;
    let Inst{6-2} = 0b11111;
    let Inst{1-0} = regToOut;
  }
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns
//===----------------------------------------------------------------------===//

let usesCustomInserter = 1 in
def Select : Scott8PseudoInst<(outs GPRegs:$dst),
                    (ins GPRegs:$src, GPRegs:$src2, i8imm:$condition),
                    "# Select PSEUDO",
                    [(set GPRegs:$dst,
                    (Scott8SelectCC GPRegs:$src, GPRegs:$src2, imm:$condition))]>;

// GlobalAddress
def : Pat<(i8 (Scott8Wrapper tglobaladdr:$dst)), (CPYri tglobaladdr:$dst)>;

// Calls
def : Pat<(Scott8Call (i8 tglobaladdr:$addr)), (CALL_PSEUDO tglobaladdr:$addr)>;
