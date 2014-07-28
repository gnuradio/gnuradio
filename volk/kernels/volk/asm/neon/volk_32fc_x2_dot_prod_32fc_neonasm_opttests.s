@ static inline void volk_32fc_x2_dot_prod_32fc_neonasm_opttests(float* cVector, const float* aVector, const float* bVector, unsigned int num_points)@
.global	volk_32fc_x2_dot_prod_32fc_neonasm_opttests
volk_32fc_x2_dot_prod_32fc_neonasm_opttests:
    push    {r4, r5, r6, r7, r8, r9, sl, fp, lr}
    vpush   {d8-d15}
    lsrs    fp, r3, #3
    sub     sp, sp, #52     @ 0x34
    mov     r9, r3
    mov     sl, r0
    mov     r7, r1
    mov     r8, r2
    vorr    q0, q7, q7
    vorr    q1, q7, q7
    vorr    q2, q7, q7
    vorr    q3, q7, q7
    vorr    q4, q7, q7
    vorr    q5, q7, q7
    veor    q6, q7, q7
    vorr    q7, q7, q7
    beq     .smallvector 
    mov     r4, r1
    mov     ip, r2
    mov     r3, #0
.mainloop:
    @mov     r6, ip
    @mov     r5, r4
    vld4.32 {d24,d26,d28,d30}, [r6]!
    @add     ip, ip, #64     @ 0x40
    @add     r4, r4, #64     @ 0x40
    vld4.32 {d16,d18,d20,d22}, [r5]!
    add     r3, r3, #1
    vld4.32 {d25,d27,d29,d31}, [r6]!
    vld4.32 {d17,d19,d21,d23}, [r5]!
    vmla.f32        q6, q8, q12
    vmla.f32        q0, q9, q12
    cmp     r3, fp
    vmls.f32        q5, q13, q9
    vmla.f32        q2, q13, q8
    vmla.f32        q7, q10, q14
    vmla.f32        q1, q11, q14
    vmls.f32        q4, q15, q11
    vmla.f32        q3, q15, q10
    bne     .mainloop 
    lsl     r3, fp, #6
    add     r8, r8, r3
    add     r7, r7, r3
.smallvector:
    vadd.f32        q3, q2, q3
    add     r3, sp, #16
    lsl     r4, fp, #3
    vadd.f32        q4, q5, q4
    cmp     r9, r4
    vadd.f32        q6, q6, q7
    vadd.f32        q1, q0, q1
    vadd.f32        q8, q6, q4
    vadd.f32        q9, q1, q3
    vst2.32 {d16-d19}, [r3 :64]
    vldr    s15, [sp, #24]
    vldr    s16, [sp, #16]
    vldr    s17, [sp, #20]
    vadd.f32        s16, s16, s15
    vldr    s11, [sp, #28]
    vldr    s12, [sp, #40]  @ 0x28
    vldr    s13, [sp, #44]  @ 0x2c
    vldr    s14, [sp, #32]
    vldr    s15, [sp, #36]  @ 0x24
    vadd.f32        s17, s17, s11
    vadd.f32        s16, s16, s12
    vadd.f32        s17, s17, s13
    vadd.f32        s16, s16, s14
    vadd.f32        s17, s17, s15
    vstr    s16, [sl]
    vstr    s17, [sl, #4]
    bls     .epilog 
    add     r5, sp, #8
.tailcase:
    ldr     r3, [r7], #8
    mov     r0, r5
    ldr     r1, [r8], #8
    add     r4, r4, #1
    ldr     ip, [r7, #-4]
    ldr     r2, [r8, #-4]
    str     ip, [sp]
    bl      __mulsc3
    vldr    s14, [sp, #8]
    vldr    s15, [sp, #12]
    vadd.f32        s16, s16, s14
    cmp     r4, r9
    vadd.f32        s17, s17, s15
    vstr    s16, [sl]
    vstr    s17, [sl, #4]
    bne     .tailcase 
.epilog:
    add     sp, sp, #52     @ 0x34
    vpop    {d8-d15}
    pop     {r4, r5, r6, r7, r8, r9, sl, fp, pc}
