#==========================================================================
#
#   The PyRISC Project
#
#   sum100.s: Calculates the sum of integers from 1 to 100
#
#   Jin-Soo Kim
#   Systems Software and Architecture Laboratory
#   Seoul National University
#   http://csl.snu.ac.kr
#
#==========================================================================


# This program computes the sum of integers from 1 to 100.
# The x31 register should have the value of 5050 (= 0x13ba) at the end.

    .text
    .align  2
    .globl  _start
_start:                         # code entry point
    li      x1, 1
    li x2, 1
    bne x1,x2, Exit
    add x1, zero, zero
    add x1, zero, zero
    add x1, zero, zero
    add x1, zero, zero
    add x1, x1, x2
    add x1, x2, x3
    add x1, x1, x3
Exit:
    ebreak

