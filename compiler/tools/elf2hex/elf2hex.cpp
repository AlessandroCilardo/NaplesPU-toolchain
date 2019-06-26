//
// This tool is specific to the GPGPU target.
// Convert a statically linked ELF executable into its in-memory representation
// and write it out in hex format compatible with a Verilog simulator's
// $readmemh function.  This overwrites the low address of
// the binary with a jump to the entry point (this clobbers the ELF header, which is
// unused).
//
//

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Support/Compiler.h>
#include <llvm/BinaryFormat/ELF.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace llvm::ELF;
using namespace llvm;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input ELF file>"), cl::init("a.out"));

static cl::opt<std::string> OutputFilename("o", cl::desc("Output hex file"),
                                           cl::value_desc("filename"));
static cl::opt<unsigned int> BaseAddress("b", cl::desc("Base Address"),
                                           cl::init(0));

int main(int argc, const char *argv[]) {
  cl::ParseCommandLineOptions(argc, argv, "elf2hex converter\n");

  FILE *inputFile = fopen(InputFilename.c_str(), "rb");
  if (!inputFile) {
    errs() << "Error opening input file\n";
    return 1;
  }

  Elf32_Ehdr eheader;
  if (fread(&eheader, sizeof(eheader), 1, inputFile) != 1) {
    errs() << "Error reading header\n";
    return 1;
  }

  if (memcmp(eheader.e_ident, ElfMagic, 4) != 0) {
    errs() << "Not an elf file\n";
    return 1;
  }
  
  if (eheader.e_machine != EM_NPU) {
    errs() << "Incorrect architecture\n";
    return 1;
  }

  if (eheader.e_phoff == 0) {
    errs() << "File has no program header\n";
    return 1;
  }

  Elf32_Phdr *pheader = (Elf32_Phdr *)calloc(sizeof(Elf32_Phdr), eheader.e_phnum);
  fseek(inputFile, eheader.e_phoff, SEEK_SET);
  if (fread(pheader, sizeof(Elf32_Phdr), eheader.e_phnum, inputFile) !=
      eheader.e_phnum) {
    errs() << "error reading program header\n";
    return 1;
  }

  // Walk throught the segments and find the highest address
  unsigned int maxAddress = 0;
  for (int segment = 0; segment < eheader.e_phnum; segment++) {
    if (pheader[segment].p_type == PT_LOAD) {
      unsigned int highAddr = pheader[segment].p_vaddr + pheader[segment].p_memsz;
      if (highAddr > maxAddress){
        maxAddress = highAddr;
      }
      if (pheader[segment].p_vaddr < BaseAddress) {
        errs() << "Program segment " << segment << " @" << pheader[segment].p_vaddr
  		    << " is before requested base address " << BaseAddress << "\n";
        return 1;
      }
    }
  }
  
  //mem_size must contain entire cache lines, i.e. 512 bits.
  long long unsigned x = maxAddress - BaseAddress;
  size_t mem_size = ((x + 64 - 1) / 64) * 64;
  size_t size_of_byte = 1;
  unsigned char *memoryImage = (unsigned char *)calloc(mem_size, size_of_byte);
  if (!memoryImage) {
    errs() << "not enough memory for program image\n";
    return 1;
  }

  for (int segment = 0; segment < eheader.e_phnum; segment++) {
    if (pheader[segment].p_type == PT_LOAD) {    
      fseek(inputFile, pheader[segment].p_offset, SEEK_SET);
      if (fread(memoryImage + pheader[segment].p_vaddr - BaseAddress, 1,
                pheader[segment].p_filesz,
                inputFile) != pheader[segment].p_filesz) {
        return 1;
      }
    }
  }
  
  //0xFF padding
  for (unsigned int i=maxAddress-BaseAddress; i<mem_size; i++){
    memoryImage[i] = 0xFF;
  }

  fclose(inputFile);


  // WITH THE LINKER SCRIPT THIS IS NOT NECESSARY ANYMORE SINCE WE ASSURE
  // THAT THE FIRST INSTRUCTION IS LOCATED AT THE 0X00000000 ADDRESS
  //----------------------
  // Convert the first word into a jump instruction to the appropriate location
  // 0x78000000 = 01111 + 000 = Format J + Opcode JMP
  // BaseAddress = Value of PC when it is executed the hex
  // eheader.e_entry = the poiter to entry point
  // To avoid overflow towards the opcode we mask (eheader.e_entry - BaseAddress)
  // with  0xffffff, since the offset must occupy the 24 lsb of the instruction
  //TODO: aggiungere controllo sulla dimensione dell'entry point
  //if ()
  /*
  *((unsigned int *)memoryImage) = 0x78000000 |
            ((eheader.e_entry - BaseAddress) & 0xffffff);
*/
  std::string basename = OutputFilename.substr(0, OutputFilename.size() - 4);

  FILE *outputFile = fopen(OutputFilename.c_str(), "wb");
  if (!outputFile) {
    errs() << "error opening output file";
    return 1;
  }
  for (unsigned int i = 0; i < mem_size; i++) {
    fprintf(outputFile, "%02x", memoryImage[i]);
    if ((i & 3) == 3)
      fprintf(outputFile, "\n");
  }
  fclose(outputFile);
  
  FILE *outputFile2 = fopen((basename + "_mem.hex").c_str(), "wb");
  if (!outputFile2) {
    errs() << "error opening output file";
    return 1;
  }
  for (unsigned int i = 63; i < mem_size; i+=64) {
    for (unsigned int j = 0; j < 64; j+=4) {
	    for (unsigned int k = 0; k < 4; k++) {
        fprintf(outputFile2, "%02x", memoryImage[i-j-k]);
	    }
    }
	fprintf(outputFile2, "\n");
  }
  fclose(outputFile2);

  FILE *outputFile3 = fopen((basename + "_mem_mango.hex").c_str(), "wb");
  bool zeroline;
  for (unsigned int i = 0; i < mem_size; i+=64) {
  zeroline = true;
    for (unsigned int j = 0; j < 64 && zeroline; j++) {
      if(memoryImage[i+j] != 0x00)
        zeroline = false;
    }
    if(!zeroline){
      fprintf(outputFile3, "0x%08x,0x", i);
      for (unsigned int j = 0; j < 64; j+=4) {
        for (unsigned int k = 0; k < 4; k++) {
//  	    fprintf(outputFile3, "%02x", memoryImage[i+j+3-k]);
          fprintf(outputFile3, "%02x", memoryImage[i+j+k]);
        }
      }
      fprintf(outputFile3, "\n");
    }
  }
  fclose(outputFile3);

  FILE *outputFile4 = fopen((basename + "_mem_standalone.hex").c_str(), "wb");
  for (int segment = 0; segment < eheader.e_phnum; segment++) {
    if (pheader[segment].p_type == PT_LOAD) {    
      unsigned int seg_start_addr = pheader[segment].p_vaddr;
      unsigned int elf_memsize = pheader[segment].p_memsz;
      // round elf_memsize to the next memory block
      elf_memsize = ((elf_memsize + 63) / 64) * 64;
      for (unsigned int blocks = 0; blocks < (elf_memsize / 64); blocks++) {
        unsigned int block_start_addr = seg_start_addr + (blocks * 64);
        fprintf(outputFile4, "0x%08x,0x", block_start_addr);

        for (unsigned int byte_off = 0; byte_off < 64; byte_off++) {
          fprintf(outputFile4, "%02x", memoryImage[block_start_addr + byte_off]);
        }
        fprintf(outputFile4, "\n");
      }
    }
  }
  fclose(outputFile4);

  free(memoryImage);
  free(pheader);

  return 0;
}
