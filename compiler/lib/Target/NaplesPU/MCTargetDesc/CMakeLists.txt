add_llvm_library(LLVMNaplesPUDesc
    NaplesPUMCTargetDesc.cpp
    NaplesPUMCAsmInfo.cpp
    NaplesPUMCCodeEmitter.cpp
    NaplesPUAsmBackend.cpp
    NaplesPUELFObjectWriter.cpp
    NaplesPUMCExpr.cpp
  )

add_dependencies(LLVMNaplesPUDesc NaplesPUCommonTableGen)
