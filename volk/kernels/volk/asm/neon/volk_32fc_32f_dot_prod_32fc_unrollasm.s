@ static inline void volk_32fc_32f_dot_prod_32fc_unrollasm ( lv_32fc_t* result, const  lv_32fc_t* input, const  float* taps, unsigned int num_points)
.global	volk_32fc_32f_dot_prod_32fc_unrollasm
volk_32fc_32f_dot_prod_32fc_unrollasm:
	@ r0 - result: pointer to output array (32fc)
	@ r1 - input: pointer to input array 1 (32fc)
	@ r2 - taps: pointer to input array 2 (32f)
	@ r3 - num_points: number of items to process

    push    {r4, r5, r6, r7, r8, r9}
    vpush   {q4-q7}
    sub     r13, r13, #56   @ 0x38
    add     r12, r13, #8
    lsrs    r8, r3, #3
    veor.32 q2, q5, q5
    veor.32 q3, q5, q5
    veor.32 q4, q5, q5
    veor.32 q5, q5, q5
    beq     .smallvector 
    vld2.32 {d20-d23}, [r1]!
    vld1.32 {d24-d25}, [r2]!
    mov     r5, #1



.mainloop:
    vld2.32 {d14-d17}, [r1]!  @ q7,q8
    vld1.32 {d18-d19}, [r2]!  @ q9

    vmul.f32        q0, q12, q10 @ real mult
    vmul.f32        q1, q12, q11 @ imag mult

    add     r5, r5, #1
    cmp     r5, r8

    vadd.f32        q4, q4, q0@ q4 accumulates real
    vadd.f32        q5, q5, q1@ q5 accumulates imag

    vld2.32 {d20-d23}, [r1]!  @ q10-q11
    vld1.32 {d24-d25}, [r2]!  @ q12

    vmul.f32        q13, q9, q7
    vmul.f32        q14, q9, q8
    vadd.f32        q2, q2, q13  @ q2 accumulates real
    vadd.f32        q3, q3, q14  @ q3 accumulates imag



    bne     .mainloop 

    vmul.f32        q0, q12, q10 @ real mult
    vmul.f32        q1, q12, q11 @ imag mult

    vadd.f32        q4, q4, q0@ q4 accumulates real
    vadd.f32        q5, q5, q1@ q5 accumulates imag


.smallvector:
    vadd.f32        q0, q2, q4
    add     r12, r13, #24
    lsl     r8, r8, #3
    vadd.f32        q1, q3, q5
    cmp     r3, r8

    vadd.f32    d0, d0, d1
    vadd.f32    d1, d2, d3
    vadd.f32    s14, s0, s1
    vadd.f32    s15, s2, s3

    vstr    s14, [r13]
    vstr    s15, [r13, #4]
    bls     .D1 
    rsb     r12, r8, r3
    lsr     r4, r12, #2
    cmp     r4, #0
    cmpne   r12, #3
    lsl     r5, r4, #2
    movhi   r6, #0
    movls   r6, #1
    bls     .L1 
    vmov.i32        q10, #0 @ 0x00000000
    mov     r9, r1
    mov     r7, r2
    vorr    q11, q10, q10

.smallloop:
    add     r6, r6, #1
    vld2.32 {d16-d19}, [r9]!
    cmp     r4, r6
    vld1.32 {d24-d25}, [r7]!
    vmla.f32        q11, q12, q8
    vmla.f32        q10, q12, q9
    bhi     .smallloop 
    vmov.i32        q9, #0  @ 0x00000000
    cmp     r12, r5
    vadd.f32        d20, d20, d21
    add     r8, r8, r5
    vorr    q8, q9, q9
    add     r1, r1, r5, lsl #3
    vadd.f32        d22, d22, d23
    add     r2, r2, r5, lsl #2
    vpadd.f32       d18, d20, d20
    vpadd.f32       d16, d22, d22
    vmov.32 r4, d18[0]
    vmov.32 r12, d16[0]
    vmov    s13, r4
    vadd.f32        s15, s13, s15
    vmov    s13, r12
    vadd.f32        s14, s13, s14
    beq     .finishreduction 
    .L1: 
    add     r12, r8, #1
    vldr    s13, [r2]
    cmp     r3, r12
    vldr    s11, [r1]
    vldr    s12, [r1, #4]
    vmla.f32        s14, s13, s11
    vmla.f32        s15, s13, s12
    bls     .finishreduction 
    add     r8, r8, #2
    vldr    s13, [r2, #4]
    cmp     r3, r8
    vldr    s11, [r1, #8]
    vldr    s12, [r1, #12]
    vmla.f32        s14, s13, s11
    vmla.f32        s15, s13, s12
    bls     .finishreduction 
    vldr    s13, [r2, #8]
    vldr    s11, [r1, #16]
    vldr    s12, [r1, #20]
    vmla.f32        s14, s13, s11
    vmla.f32        s15, s13, s12

.finishreduction:
    vstr    s14, [r13]
    vstr    s15, [r13, #4]
    .D1:
    ldr     r3, [r13]
    str     r3, [r0]
    ldr     r3, [r13, #4]
    str     r3, [r0, #4]
    add     r13, r13, #56   @ 0x38
    vpop    {q4-q7}
    pop     {r4, r5, r6, r7, r8, r9}
    bx      r14


