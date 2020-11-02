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
    li		a5,8
    blt		a2,a5,rank0
    rank1:
        lw		t0,-20(sp)	# rank8-15
        sub		a2,a2,a5
        sub		a5,a5,a2
        j		rankend
    rank0:
        lw		t0,-16(sp)	# rank0-7
        sub		a5,a5,a2
        j		rankend
    rankend:
        slli	a2,a2,2
        sll		a2,t0,a2
        srli	a2,a2,28
    ret

decode:    
    bnez	a1,start
    li		a0,0
    ret
    
    start:
    sw		a0,-36(sp)	# const char *inp
    sw		a1,-4(sp)	# int inbytes
    sw		a2,-8(sp)	# char *outp
    sw		a3,-12(sp)	# int outbytes
    
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
    sw		a2,-24(sp)	# padding
    lw		a3,-4(sp)
    addi	a3,a3,-4
    slli	a3,a3,1
    addi	a3,a3,-1	# inNibbles
    slli	a4,a0,4		# stream
    li		a7,0		# outBibbles 
    li		a1,0		# lenBit
    li		a0,0		# body
    li		t1,7		# counter
    
    loop3:
        addi	a3,a3,-1
        addi	t1,t1,-1
        slli	a0,a0,4
        srli	a5,a4,28
        add		a0,a0,a5
        slli	a4,a4,4
        addi	a1,a1,4
        bnez	t1,cont3
        
        li		t1,8
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
        bnez	a3,huff
        lw		a2,-24(sp)	# padding
        srl		a0,a0,a2
        sub		a1,a1,a2
        huff:
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
                slli	a6,a6,4
                add		a6,a6,a2
                add		a7,a7,1

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
                    slli	a6,a6,4
                    add		a6,a6,a2
                    add		a7,a7,1

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
                    slli	a6,a6,4
                    add		a6,a6,a2
                    add		a7,a7,1

                    j		huff
        
        break:
        andi	a2,a7,7
        bnez	a2,continue3
        
        sw		a0,-28(sp)	# a0
        sw		a1,-32(sp)	# a1
        
        mv		a0,a6
        call	swap
        mv		a6,a0
        lw		a2,-8(sp)	# char *outp
        
        sw		a6,0(a2)
        li		a6,0
        addi	a2,a2,4
        sw		a2,-8(sp)
        
        lw		a0,-28(sp)	# a0
        lw		a1,-32(sp)	# a1
        
    continue3:
        bnez	a3,loop3    
        
    andi	a2,a7,7
    beq		a2,zero,finish
    slli	a2,a2,2
    sll		a0,a6,a2
    call	swap
    lw		a2,-8(sp)	# char *outp
    sw		a0,0(a2)
    li		a6,0
    
finish:
	ret
