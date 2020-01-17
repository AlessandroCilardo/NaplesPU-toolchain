					.text
                    .globl _vectors
                    .align 4
                    .type _vectors,@function
_vectors:
					jmpsr isr0