@ static inline void volk_32f_x2_dot_prod_32f_neonasm_opts(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
    @ r0 = cVector
    @ r1 = aVector
    @ r2 = bVector
    @ r3 = num_points
    .global    volk_32f_x2_dot_prod_32f_neonasm_opts
volk_32f_x2_dot_prod_32f_neonasm_opts:
     push    {r4, r5, r6, r7, r8, r9, r10, r11}
    @ sixteenth_points = num_points / 16
     lsrs       r8, r3, #4
     sub        r13, r13, #16 @ subtracting 16 from stack pointer?, wat?
    @ 0 out neon accumulators
     veor       q0, q3, q3
     veor       q1, q3, q3
     veor       q2, q3, q3
     veor       q3, q3, q3
     beq        .smallvector @ if less than 16 points skip main loop
     mov        r7, r2  @ copy input ptrs
     mov        r6, r1  @ copy input ptrs
     mov        r5, #0  @ loop counter
.mainloop:
     vld4.32    {d16,d18,d20,d22}, [r6]!
     add        r5, r5, #1 @ inc loop counter
     cmp        r5, r8     @ loop counter < sixteenth_points?
     vld4.32    {d24,d26,d28,d30}, [r7]!
     vld4.32    {d17,d19,d21,d23}, [r6]!
     vld4.32    {d25,d27,d29,d31}, [r7]!
     vmla.f32   q3, q8, q12
     vmla.f32   q0, q13, q9
     vmla.f32   q1, q14, q10
     vmla.f32   q2, q15, q11
     bne        .mainloop
     lsl        r12, r8, #6 @ r12=r8/64
     add        r1, r1, r12
     add        r2, r2, r12
.smallvector: @ actually this can be skipped for small vectors
     vadd.f32   q3, q3, q0
     lsl        r8, r8, #4 @ sixteenth_points * 16
     cmp        r3, r8     @ num_points < sixteenth_points*16?
     vadd.f32   q2, q1, q2
     vadd.f32   q3, q2, q3 @ sum of 4 accumulators in to q3
     vadd.f32   s15, s12, s15 @ q3 is s12-s15, so reduce to a single float
     vadd.f32   s15, s15, s13
     vadd.f32   s15, s15, s14
     bls        .done      @ if vector is multiple of 16 then finish
     sbfx       r11, r1, #2, #1 @ check alignment
     rsb        r9, r8, r3
     and        r11, r11, #3
     mov        r6, r1
     cmp        r11, r9
     movcs      r11, r9
     cmp        r9, #3
     movls      r11, r9
     cmp        r11, #0
     beq        .nothingtodo
     mov        r5, r2
     mov        r12, r8
.dlabel5:
     add        r12, r12, #1
     vldmia     r6!, {s14}
     rsb        r4, r8, r12
     vldmia     r5!, {s13}
     cmp        r4, r11
     vmla.f32   s15, s13, s14
     mov        r7, r6
     mov        r4, r5
     bcc        .dlabel5
     cmp        r9, r11
     beq        .done
.dlabel8:
     rsb        r9, r11, r9
     lsr        r8, r9, #2
     lsls       r10, r8, #2
     beq        .dlabel6
     lsl        r6, r11, #2
     veor       q8, q8, q8
     add        r1, r1, r6
     add        r6, r2, r6
     mov        r5, #0
.dlabel9:
     add        r5, r5, #1
     vld1.32    {d20-d21}, [r6]!
     cmp        r5, r8
     vld1.64    {d18-d19}, [r1 :64]!
     vmla.f32   q8, q10, q9
     bcc        .dlabel9
     vadd.f32   d16, d16, d17
     lsl        r2, r10, #2
     veor       q9, q9, q9
     add        r7, r7, r2
     vpadd.f32  d6, d16, d16
     add        r4, r4, r2
     cmp        r9, r10
     add        r12, r12, r10
     vadd.f32   s15, s15, s12
     beq        .done
.dlabel6:
     mov        r2, r7
.dlabel7:
     add        r12, r12, #1
     vldmia     r2!, {s13}
     cmp        r3, r12
     vldmia     r4!, {s14}
     vmla.f32   s15, s13, s14
     bhi        .dlabel7
.done:
     vstr       s15, [r0]
     add        r13, r13, #16
     pop        {r4, r5, r6, r7, r8, r9, r10, r11}
     bx         lr @ lr is the return address
.nothingtodo:
     mov        r12, r8
     mov        r4, r2
     mov        r7, r1
     b          .dlabel8

