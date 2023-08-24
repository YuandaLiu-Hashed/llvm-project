add_llvm_component_group(Scott8)

set(LLVM_TARGET_DEFINITIONS Scott8.td)

tablegen(LLVM Scott8GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM Scott8GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Scott8GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Scott8GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Scott8GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Scott8GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM Scott8GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Scott8GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(Scott8CommonTableGen)

add_llvm_target(Scott8CodeGen
  Scott8AsmPrinter.cpp
  Scott8InstrInfo.cpp
  Scott8ISelDAGToDAG.cpp
  Scott8ISelLowering.cpp
  Scott8FrameLowering.cpp
  Scott8MachineFunctionInfo.cpp
  Scott8RegisterInfo.cpp
  Scott8Subtarget.cpp
  Scott8TargetMachine.cpp
  Scott8MCInstLower.cpp
  Scott8TargetObjectFile.cpp
  Scott8Pass_BundleJcc.cpp
  Scott8Pass_ExpandPostRAPsudo.cpp
  Scott8Pass_InsertCLF.cpp
  Scott8Pass_InsertInitialStackState.cpp

  LINK_COMPONENTS
  AsmPrinter 
  CodeGen 
  Core 
  MC 
  SelectionDAG
  Scott8Desc 
  Scott8Info 
  Support 
  Target

  ADD_TO_COMPONENT
  Scott8
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
