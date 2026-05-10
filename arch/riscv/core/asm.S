
	.section .text
	.globl get_mxlen
	.type get_mxlen,@function
get_mxlen:
    li t0, 2
    slli t0,t0,31
    bnez t0,1f
    li a0, 32
    ret
1:  
    slli t0,t0,16
    slli t0,t0,16
    bnez t0,2f
    li a0, 64
    ret
2:  
    li a0, 128
    ret