#
# Copyright 2016-2017 Edoardo Fusella
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#
# C runtime startup code. When the processor boots, only hardware thread 0 is
# running. It begins execution at _start, which sets up the stack, calls global
# constructors, and jumps to main(). If the program starts the other hardware
# threads, they also begins execution at _start, but they skip calling global
# constructors and jump directly to main.
#
# Memory map:
# 00000000   +---------------+
#            |   code/data   |
# 	     +---------------+
#            |     stacks    |
# 00200000   +---------------+
#            |  framebuffer  |
# 0032C000   +---------------+
#            |     heap      |
#            +---------------+
#

                    .text
                    .globl _start
                    .align 4
                    .type _start,@function
_start:
		    movei s2, 5
		    movei s3, 11
		    write_cr s2, s3	# Set Booting mode
		    
		    moveih rm, 0
		    moveil rm, 0xFFFF # enable all lanes
                    
		    # Set up stack
		    movei s1, 0
		    movei s2, 2
                    read_cr s1, s1        # get my tile ID (i)
                    read_cr s2, s2        # get my thread ID (j)

		    # Load constants from linker script
		    leah s4, stacks_dim
		    leal s4, stacks_dim
		    leah s5, threads_per_core
		    leal s5, threads_per_core
		    
			leah  s0, stacks_base
		    leal  s0, stacks_base

		    # Calculate stack top address as stacks_base + ((CID * threads) + TID + 1) * stacks_dim
		    mull_i32 s3, s1, s5
		    add_i32 s3, s3, s2
		    addi s3, s3, 1
		    mull_i32 s3, s3, s4
		    add_i32 s0, s0, s3

		    leah sp, stacks_base
		    leal sp, stacks_base
		    leah fp, stacks_base
		    leal fp, stacks_base

                    # Only thread 0 does initialization.  Skip for other
                    # threads, which only arrive here after thread 0 has
                    # completed initialization and started them).
            bnez s2, do_main
		    bnez s1, preload_scratchpad

		    # only tile 0 thread 0 must init C constructors
                    # Call global initializers
                    leah  s24, init_array_start
                    leal  s24, init_array_start
                    leah  s25, init_array_end
                    leal  s25, init_array_end
init_loop:          cmpge_i32 s0, s24, s25    # End of array?
										bnez s0, preload_scratchpad   # If so, exit loop
                    load32 s0, (s24)        # Load ctor address
                    addi s24, s24, 4        # Next array index
                    jmpsr s0                 # Call constructor
                    jmp init_loop

					# each thread 0 must load scratchpad data
preload_scratchpad: xor_i32 s0, s0, s0
					xor_i32 s1, s1, s1
					leah s0, __scratchpad_data_start
					leal s0, __scratchpad_data_start
					leah s1, __scratchpad_data_end
					leal s1, __scratchpad_data_end
					moveih s2, 0		    
					moveil s2, 0
					sub_i32 s3, s1, s0

preload_scratchpad_vector: cmplti s4, s3, 64 # 64 or more bytes left?
	  bnez s4, preload_scratchpad_words
          load_v16i32 v0, (s0)
          store_v16i32_scratchpad v0, (s2)
          addi s0, s0, 64
          addi s2, s2, 64
          subi s3, s3, 64
          jmp preload_scratchpad_vector

#preload_scratchpad_long: cmplti s4, s3, 8     # 8 or more bytes left?
#          bnez s4, preload_scratchpad_words 
#          load64 s4_64, (s0)
#          store64_scratchpad s4_64, (s2)
#          addi s0, s0, 8
#          addi s2, s2, 8
#          subi s3, s3, 8
#          jmp preload_scratchpad_long

preload_scratchpad_words: cmplti s4, s3, 4     # 4 or more bytes left?
          bnez s4, preload_scratchpad_remain_bytes
          load32 s4, (s0)
          store32_scratchpad s4, (s2)
          addi s0, s0, 4
          addi s2, s2, 4
          subi s3, s3, 4
          jmp preload_scratchpad_words

preload_scratchpad_remain_bytes: cmplti s4, s3, 1     # 1 or more bytes left?
		  		bnez s4, do_main
          load32_u8 s4, (s0)
          store32_8_scratchpad s4, (s2)
          addi s0, s0, 1
          addi s2, s2, 1
          subi s3, s3, 1
          jmp preload_scratchpad_remain_bytes
		    
do_main:            
	  movei s0, 12
	  movei s1, 13
          
    read_cr s0, s0     # Set argc
	  read_cr s1, s1     # Set argv
	  movei s2, 1
          movei s3, 11    
	  write_cr s2, s3	# Set Running mode
          jmpsr main
    movei s1, 13
    write_cr s0, s1	# Set argv with main return
	  movei s1, 0
	  moveih s2, 0xFFFF
	  moveil s2, 0xFFC0
	  store32 s1, (s2)
	  flush s2
	  #barrier s1, s1
          movei s2, 2
	  movei s3, 11
	  write_cr s2, s3	# Set End mode
    #moveih s4, -1
    #moveil s4, 0xF0F8
    #load32  s4, (s4)
    #movei s1, 13
    #write_cr s4, s1	# Set argv with main return
					
1:        jmp 1b

init_array_end:     .long __init_array_end
init_array_start:   .long __init_array_start
thread_halt_addr:   .long 0xffff0064

#.weak __scratchpad_data_end
#__scratchpad_data_end: .long 0x0
#
#.weak __scratchpad_data_start
#__scratchpad_data_start: .long 0x0
