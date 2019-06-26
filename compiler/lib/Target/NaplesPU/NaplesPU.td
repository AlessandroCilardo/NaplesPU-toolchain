//===-- NaplesPU.td - Describe the NaplesPU Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "NaplesPURegisterInfo.td"
include "NaplesPUCallingConv.td"
include "NaplesPUInstrInfo.td"

def NaplesPUInstrInfo : InstrInfo;

def NaplesPUAsmParser : AsmParser {
  bit ShouldEmitMatchRegisterName = 1;
}

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def NaplesPUAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter = 1;
}

//TODO: Currently disabled because it causes bad code gen with large stack frames 
//def : Processor<"naplespu", NaplesPUItineraries, []>;
def : Processor<"naplespu", NoItineraries, []>;

def NaplesPU : Target {
  // Pull in Instruction Info:
  let InstructionSet = NaplesPUInstrInfo;
  let AssemblyParsers  = [NaplesPUAsmParser];
  let AssemblyWriters = [NaplesPUAsmWriter];
}
