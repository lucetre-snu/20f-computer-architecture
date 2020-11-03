#----------------------------------------------------------------
#
#  4190.308 Computer Architecture (Fall 2020)
#
#  Project #3: RISC-V Assembly Programming
#
#  October 26, 2020
#
#  Injae Kang (abcinje@snu.ac.kr)
#  Sunmin Jeong (sunnyday0208@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#----------------------------------------------------------------


	.text
	.align	2

#---------------------------------------------------------------------
#  int decode(const char *inp, int inbytes, char *outp, int outbytes)
#---------------------------------------------------------------------
	.globl	decode
    
swap:
    andi	a1,a0,255
    slli	a1,a1,8
    srli	a0,a0,8
    andi	a2,a0,255
    add		a1,a1,a2
    slli	a1,a1,8
    srli	a0,a0,8
    andi	a2,a0,255
    add		a1,a1,a2
    slli	a1,a1,8
    srli	a0,a0,8
    andi	a2,a0,255
    add		a1,a1,a2
    mv		a0,a1
    ret

rank:
    # ra as local var
    sw		ra,-56(sp)	# store_ra
    li		a5,8
    blt		a2,a5,rank0
    rank1:
        lw		ra,-20(sp)	# rank8-15
        sub		a2,a2,a5
        sub		a5,a5,a2
        j		rankend
    rank0:
        lw		ra,-16(sp)	# rank0-7
        sub		a5,a5,a2
        j		rankend
    rankend:
        slli	a2,a2,2
        sll		a2,ra,a2
        srli	a2,a2,28
    lw		ra,-56(sp)	# load_ra
    ret

store:
    sw		ra,-56(sp)	# store_ra
    sw		a0,-28(sp)	# a0
    sw		a1,-32(sp)	# a1
    lw		a5,-48(sp)	# load a5
    mv		a0,a5
    call	swap
    mv		a5,a0
    lw		a2,-8(sp)	# char *outp

    sw		a5,0(a2)
    li		a5,0
    addi	a2,a2,4
    sw		a2,-8(sp)
    
    sw		a5,-48(sp)	# store a5
    lw		a0,-28(sp)	# a0
    lw		a1,-32(sp)	# a1
    lw		ra,-56(sp)	# load_ra
    ret

decode:    
    bnez	a1,start
    li		a0,0
    ret
    
    start:
    sw		ra,-52(sp)	# ra
    sw		a0,-36(sp)	# const char *inp
    slli	a1,a1,1
    addi	a1,a1,-1
    sw		a1,-4(sp)	# int inNibbles
    sw		a2,-8(sp)	# char *outp
    slli	a3,a3,1
    sw		a3,-12(sp)	# int outNibbles
    
    # const char *inp
    mv		a5,a0
    
    # little to big endian
    lw		a5,0(a5)
    mv		a0,a5
    call	swap
    mv		a5,a0
    
    # rank table (a3:rank8-15, a4:freq, a5:rank0-7)
    li		a0,32
    li		a1,15
    slli	a1,a1,28
    loop1:
        addi	a0,a0,-4
        and		a2,a1,a5
        srl		a2,a2,a0
        srli	a1,a1,4
        li		a3,1
        sll		a3,a3,a2
        add		a4,a3,a4
        bnez	a0,loop1
    li		a0,16
    li		a1,0
    li		a3,0
    loop2:
        li		a2,1
        sll		a2,a2,a1
        and		a2,a2,a4
        bnez	a2,continue2
        slli	a3,a3,4
        add		a3,a3,a1
    continue2:
        addi	a1,a1,1
        bne		a0,a1,loop2
    
    sw		a5,-16(sp)	# rank0-7
    sw		a3,-20(sp)	# rank8-15
    
    # little to big endian
    lw		a5,-36(sp)	# const char *inp
    addi	a5,a5,4
    lw		a0,0(a5)
    sw		a5,-36(sp)
    call	swap
    
    li		a2,15
    slli	a2,a2,28
    and		a2,a1,a2
    srli	a2,a2,28
    addi	a2,a2,-4
    bge		zero,a2,else0
    addi	a2,a2,-4
    lw		a1,-4(sp)	# load inNibbles
    addi	a1,a1,-1
    sw		a1,-4(sp)	# store inNibbles
else0:
    addi	a2,a2,4
    sw		a2,-24(sp)	# padding > local
    sw		zero,-48(sp)# output a5
    sw		zero,-44(sp)# outlen a5
    li		a3,9		# inNibbles
    slli	a4,a0,4		# stream > local
    li		a1,0		# lenBit
    li		a0,0		# body
    loop3:
        addi	a3,a3,1
        slli	a0,a0,4
        srli	a5,a4,28
        add		a0,a0,a5
        slli	a4,a4,4
        addi	a1,a1,4
        and		a5,a3,7
        bnez	a5,cont3 # nothing to read from stream
        
        sw		a0,-28(sp)	# a0
        sw		a1,-32(sp)	# a1
        
        lw		a5,-36(sp)
        addi	a5,a5,4
        sw		a5,-36(sp)
        lw		a0,0(a5)
        call	swap
        mv		a4,a0
        
        lw		a0,-28(sp)	# a0
        lw		a1,-32(sp)	# a1
    cont3:
        lw		a5,-4(sp)	# load a5(inNibbles)
        addi	a5,a5,1
        bgt		a5,a3,huff    
        
        # final nibble
        
        lw		a2,-24(sp)	# padding
        
        srl		a0,a0,a2
        sub		a1,a1,a2
        li		a4,0
        
        huff:
            lw		a5,-48(sp)	# load a5(output)
            addi	a5,a1,-1
            blt		a5,zero,break
            li		a2,1
            sll		a2,a2,a5
            and		a2,a2,a0
            bnez	a2,O
            Z:
                addi	a5,a1,-3
                blt		a5,zero,break
                mv		a1,a5
                li		a2,3
                sll		a2,a2,a5
                and		a2,a2,a0
                srl		a2,a2,a5
                call	rank
                
                lw		a5,-48(sp)	# load a5(output)
                slli	a5,a5,4
                add		a5,a5,a2
                sw		a5,-48(sp)	# store a5(output)
                
                lw		a5,-44(sp)	# load a5(outlen)
                lw		ra,-12(sp)	# load outNibbles
                bge		ra,a5,continueZ
                li		a0,-1
                j		done
                    
                continueZ:
                add		a5,a5,1
                andi	a2,a5,7
                sw		a5,-44(sp)	# store a5(outlen)
                
                bnez	a2,elseZ
                
                call		store
                elseZ:
                j		huff
            O:
                addi	a5,a1,-2
                blt		a5,zero,break

                li		a2,1
                sll		a2,a2,a5
                and		a2,a2,a0
                bnez	a2,OO
                OZ:
                    addi	a5,a1,-4
                    blt		a5,zero,break
                    mv		a1,a5
                    li		a2,3
                    sll		a2,a2,a5
                    and		a2,a2,a0
                    srl		a2,a2,a5
                    addi	a2,a2,4
                    call	rank
                    
                    lw		a5,-48(sp)	# load a5(output)
                    slli	a5,a5,4
                    add		a5,a5,a2
                    sw		a5,-48(sp)	# store a5(output)
                    
                    lw		a5,-44(sp)	# load a5(outlen)
                    lw		ra,-12(sp)	# load outNibbles
                    bge		ra,a5,continueZ
                    li		a0,-1
                    j		done
                    
                    continueOZ:
                    add		a5,a5,1
                    andi	a2,a5,7
                    sw		a5,-44(sp)	# store a5(outlen)
                
                    bnez	a2,elseOZ
        
                    call		store
                    elseOZ:
                    j		huff
                OO:
                    addi	a5,a1,-5
                    blt		a5,zero,break
                    mv		a1,a5
                    li		a2,7
                    sll		a2,a2,a5
                    and		a2,a2,a0
                    srl		a2,a2,a5
                    addi	a2,a2,8
                    call	rank
                    
                    lw		a5,-48(sp)	# load a5(output)
                    slli	a5,a5,4
                    add		a5,a5,a2
                    sw		a5,-48(sp)	# store a5(output)
                    
                    lw		a5,-44(sp)	# load a5(outlen)
                    lw		ra,-12(sp)	# load outNibbles
                    bge		ra,a5,continueZ
                    li		a0,-1
                    j		done
                    
                    continueOO:
                    add		a5,a5,1
                    andi	a2,a5,7
                    sw		a5,-44(sp)	# store a5(outlen)
                    
                    bnez	a2,elseOO
        
                    call		store
                    elseOO:
                    j		huff
        
        break:
        lw		a5,-4(sp)	# load a5(inNibbles)
        addi	a5,a5,1
        bge		a5,a3,loop3    
          
    lw		a5,-44(sp)	# load a5(outlen)
    andi	a2,a5,7
    beq		a2,zero,finish
    
    lw		a5,-48(sp)	# load a5(output)
    li		a3,8
    sub		a2,a3,a2
    slli	a2,a2,2
    sll		a0,a5,a2
    call	swap
    lw		a2,-8(sp)	# char *outp
    sw		a0,0(a2)
    
finish:
    lw		a5,-44(sp)	# load a5(outlen)
    srli	a0,a5,1
done:
    lw		ra,-52(sp)	# ra
	ret
