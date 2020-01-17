stacks_dim = 16K;
stacks_base = 0x20100000;
threads_per_core = 0x8;
start_address = 0x20000000;

SECTIONS
{

  .text.vectors : {
    . = start_address + 0x00000380;
    *vectors.o(.text)
  }

  . = start_address + 0x00000400;

  .text.start : {
    *crt0.o(.text)
  }
  .text : { *(.text) }

  . = start_address + 0x00800000;
  .data : { *(.data) }

  . = ALIGN(64);
  scratchpad : {
    *(scratchpad)
  }

  . = ALIGN(64);
  .bss : { *(.bss) }

  . = ALIGN(64);
  .eh_frame : { *(.eh_frame) }

}
