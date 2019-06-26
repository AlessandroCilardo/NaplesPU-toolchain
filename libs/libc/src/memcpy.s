# s0 - dest
# s1 - src
# s2 - count

                    	.global memcpy
                    	.type memcpy,@function
memcpy:             	move_i32 s6, s0                # Save source pointer, which we will return

                    	# Check if the source and dest have the same alignment
                    	# modulo 64. If so, we can do block vector copy
                    	andi s3, s0, 63
                    	andi s4, s1, 63
                    	cmpeq_i32 s5, s3, s4
                    	beqz s5, copy_word_check    # Not aligned, see if we can copy words

                        # ...Falls through, we can do vector copies

                    	# There may be leading bytes before alignment.  Copy up to that.
copy_vector_lead_in: 	andi s4, s0, 63          # Aligned yet?
                    	beqz s4, copy_vector  # Yes, time to do big copies
                    	beqz s2, copy_done    # Bail if we are done.
                    	load32_u8 s4, (s1)
                    	store32_8 s4, (s0)
                    	addi s0, s0, 1
                    	addi s1, s1, 1
                    	subi s2, s2, 1
                    	jmp copy_vector_lead_in

                    	# Copy entire vectors at a time
copy_vector:        	cmpulti s4, s2, 64      # 64 or more bytes left?
                    	bnez s4, copy_words    # No, attempt to copy words
                    	load_v16i32 v0, (s1)
                    	store_v16i32 v0, (s0)
                    	addi s0, s0, 64
                    	addi s1, s1, 64
                    	subi s2, s2, 64
                    	jmp copy_vector

                    	# Check the source and dest have the same alignment
                    	# modulo 4.  If so, we can copy 32 bits at a time.
copy_word_check:    	andi s3, s0, 3
                    	andi s4, s1, 3
                    	cmpeq_i32 s5, s3, s4
                    	beqz s5, copy_remain_bytes    # Not aligned, need to do it the slow way

copy_word_lead_in:    	andi s4, s0, 3           # Aligned yet?
                    	beqz s4, copy_words   # If yes, start copying
                    	beqz s2, copy_done    # Bail if we are done
                    	load32_u8 s4, (s1)
                    	store32_8 s4, (s0)
                    	addi s0, s0, 1
                    	addi s1, s1, 1
                    	subi s2, s2, 1
                    	jmp copy_word_lead_in

copy_words:            	cmpulti s4, s2, 4           # 4 or more bytes left?
                    	bnez s4, copy_remain_bytes # If not, copy tail
                    	load32 s4, (s1)
                    	store32 s4, (s0)
                    	addi s0, s0, 4
                    	addi s1, s1, 4
                    	subi s2, s2, 4
                    	jmp copy_words

                    	# Perform byte copy of whatever is remaining
copy_remain_bytes:    	beqz s2, copy_done
                    	load32_u8 s4, (s1)
                    	store32_8 s4, (s0)
                    	addi s0, s0, 1
                    	addi s1, s1, 1
                    	subi s2, s2, 1
                    	jmp copy_remain_bytes

copy_done:            	move_i32 s0, s6        # Get source pointer to return
                    	jret
