add_llvm_component_library(LLVMScott8Info
  Scott8TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  Scott8
  )
