@ static inline void volk_32fc_x2_dot_prod_32fc_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_32fc_x2_dot_prod_32fc_neonasm
volk_32fc_x2_dot_prod_32fc_neonasm:
    push    {r4, r5, r6, r7, r8, lr}
    vpush   {q0-q7}
    vpush   {q8-q15}
    mov r8, r3          @ hold on to num_points (r8)
    @ zero out accumulators -- leave 1 reg in alu
    veor    q8, q15, q15
    mov r7, r0          @ (r7) is cVec
    veor    q9, q15, q15
    mov r5, r1          @ (r5) is aVec
    veor    q10, q15, q15
    mov r6, r2          @ (r6) is bVec
    veor    q11, q15, q15
    lsrs    r3, r3, #3  @ eighth_points (r3) = num_points/8
    veor    q12, q15, q15
    mov r12, r2         @ (r12) is bVec
    veor    q13, q15, q15
    mov r4, r1          @ (r4) is aVec
    veor    q14, q15, q15
    veor    q15, q15, q15
    beq .smallvector @ nathan optimized this file based on an objdump
    @ but I don't understand this jump. Seems like it should go to loop2
    @ and smallvector (really vector reduction) shouldn't need to be a label
    mov r2, #0          @ 0 out r2 (now number)
.loop1:
    add r2, r2, #1      @ increment number
    vld4.32 {d0,d2,d4,d6}, [r12]! @ q0-q3
    cmp r2, r3          @ is number < eighth_points
    @pld [r12, #64]   
    vld4.32 {d8,d10,d12,d14}, [r4]! @ q4-q7
    @pld [r4, #64]  
    vmla.f32    q12, q4, q0 @ real (re*re)
    vmla.f32    q14, q4, q1 @ imag (re*im)
    vmls.f32    q15, q5, q1 @ real (im*im)
    vmla.f32    q13, q5, q0 @ imag (im*re)

    vmla.f32    q8, q2, q6 @ real (re*re)
    vmla.f32    q9, q2, q7 @ imag (re*im)
    vmls.f32    q10, q3, q7 @ real (im*im)
    vmla.f32    q11, q3, q6 @ imag (im*re)
    bne .loop1
    lsl r2, r3, #3      @ r2 = eighth_points * 8
    add r6, r6, r2      @ bVec = bVec + eighth_points -- whyyyyy gcc?!?
    add r5, r5, r2      @ aVec = aVec + eighth_points
    @ q12-q13 were original real accumulators
    @ q14-q15 were original imag accumulators
    @ reduce 8 accumulators down to 2 (1 real, 1 imag)
    vadd.f32    q8, q10, q8 @ real + real
    vadd.f32    q11, q11, q9 @ imag + imag
    vadd.f32    q12, q12, q15 @ real + real
    vadd.f32    q14, q14, q13 @ imag + imag
    vadd.f32    q8, q8, q12
    vadd.f32    q9, q9, q14
.smallvector:
    lsl r4, r3, #3
    cmp r8, r4
    vst2.32 {d16-d19}, [sp :64] @ whaaaaat? no way this is necessary!
    vldr    s15, [sp, #8]
    vldr    s17, [sp]
    vldr    s16, [sp, #4]
    vadd.f32    s17, s17, s15
    vldr    s11, [sp, #12]
    vldr    s12, [sp, #24]
    vldr    s13, [sp, #28]
    vldr    s14, [sp, #16]
    vldr    s15, [sp, #20]
    vadd.f32    s16, s16, s11
    vadd.f32    s17, s17, s12
    vadd.f32    s16, s16, s13
    vadd.f32    s17, s17, s14
    vadd.f32    s16, s16, s15
    vstr    s17, [r7]
    vstr    s16, [r7, #4]
    bls .done
.loop2:
    mov r3, r6
    add r6, r6, #8
    vldr    s0, [r3]
    vldr    s1, [r6, #-4]
    mov r3, r5
    add r5, r5, #8
    vldr    s2, [r3]
    vldr    s3, [r5, #-4]
    bl  __mulsc3            @ GCC/Clang built-in. Portability?
    add r4, r4, #1
    cmp r4, r8
    vadd.f32    s17, s17, s0
    vadd.f32    s16, s16, s1
    vstr    s17, [r7]
    vstr    s16, [r7, #4]
    bne .loop2
.done: 
    vpop    {q8-q15}
    vpop    {q0-q7}
    pop {r4, r5, r6, r7, r8, pc}

