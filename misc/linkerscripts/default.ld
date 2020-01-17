stacks_dim = 16K;
stacks_base = 0x80000;
threads_per_core = 0x8;

SECTIONS
{
  . = 0x00000380;
  .text.vectors : {
    *vectors.o(.text)
    . = 0x00000400;
  }

  .text.start : {
    *crt0.o(.text)
  }
  .text : { *(.text) }

  . = 0x00800000;
  .data : { *(.data) }

  . = ALIGN(64);
  scratchpad : { *(scratchpad) }

  .bss : { *(.bss) }

  .eh_frame : { *(.eh_frame) }

}
