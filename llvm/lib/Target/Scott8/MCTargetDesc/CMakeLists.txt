add_llvm_component_library(LLVMScott8Desc
  Scott8AsmBackend.cpp
  Scott8InstPrinter.cpp
  Scott8MCAsmInfo.cpp
  Scott8MCCodeEmitter.cpp
  Scott8MCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Scott8Info
  Support

  ADD_TO_COMPONENT
  Scott8
  )