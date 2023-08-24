//===-- Scott8RegisterInfo.td - Scott8 Register defs ---------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the Scott8 register file
//===----------------------------------------------------------------------===//

// General register definition
class Scott8Reg<bits<16> Enc, string n> : Register<n> {
  let Namespace = "Scott8";
  let HWEncoding = Enc;
}

// General purpose registers
def R0 : Scott8Reg<0, "R0">;
def R1 : Scott8Reg<1, "R1">;
def R2 : Scott8Reg<2, "R2">;

// Special purpose registers
def SP : Scott8Reg<3, "R3">; // R3 will serve as the stack register.
def FR : Scott8Reg<4, "FR">; // Flags (isn't encoded by any instr)
def PC : Scott8Reg<5, "PC">; // Program counter (isn't encoded by any instr)

def FlagReg : RegisterClass<"Scott8", [i8], 8, (add FR)>;
def ProgramCounterReg : RegisterClass<"Scott8", [i8], 8, (add PC)>;
def GPRegs : RegisterClass<"Scott8", [i8], 8, (add R0, R1, R2, SP)>;
def NonStackGPRegs : RegisterClass<"Scott8", [i8], 8, (add R0, R1, R2)>;
def StackGPRegs :  RegisterClass<"Scott8", [i8], 8, (add SP)>;
