set(LLVM_TARGET_DEFINITIONS NaplesPU.td)

tablegen(LLVM NaplesPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM NaplesPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM NaplesPUGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM NaplesPUGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM NaplesPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM NaplesPUGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM NaplesPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM NaplesPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM NaplesPUGenCallingConv.inc -gen-callingconv)
add_public_tablegen_target(NaplesPUCommonTableGen)

add_llvm_target(NaplesPUCodeGen
  NaplesPUAsmPrinter.cpp
  NaplesPUInstrInfo.cpp
  NaplesPUISelDAGToDAG.cpp
  NaplesPUISelLowering.cpp
  NaplesPUFrameLowering.cpp
  NaplesPUMachineFunctionInfo.cpp
  NaplesPURegisterInfo.cpp
  NaplesPUSubtarget.cpp
  NaplesPUTargetMachine.cpp
  NaplesPUMCInstLower.cpp
  NaplesPUTargetObjectFile.cpp
  NaplesPUTargetTransformInfo.cpp
  )

add_dependencies(LLVMNaplesPUCodeGen intrinsics_gen)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(AsmParser)
add_subdirectory(InstPrinter)
add_subdirectory(Disassembler)

