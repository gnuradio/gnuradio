@ static inline void volk_32fc_x2_multiply_32fc_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_32fc_x2_multiply_32fc_neonasm
volk_32fc_x2_multiply_32fc_neonasm:
    push    {r4, r5, r6, r7, r8, r9, r14}
    lsrs    r7, r3, #2
    @ r0 is c vector
    @ r1 is a vector
    @ r2 is b vector
    @ r3 is num_points
    @ r7 is quarter_points
    beq     .smallvector
    mov     r5, #0
.mainloop:
   vld2.32   {d24-d27}, [r1]!  @ ar=q12, ai=q13
   add       r5, r5, #1
   cmp       r5, r7
   vld2.32   {d20-d23}, [r2]!  @ br=q10, bi=q11
   pld       [r1]
   pld       [r2]
   vmul.f32  q0, q12, q10 @ q15 = ar*br
   vmul.f32  q1, q13, q11 @ q11 = ai*bi
   vmul.f32  q2, q12, q11 @ q14 = ar*bi
   vmul.f32  q3, q13, q10 @ q12 = ai*br
   vsub.f32  q8, q0, q1  @ real
   vadd.f32  q9, q2, q3  @ imag
   vst2.32   {d16-d19}, [r0]!
   bne     .mainloop 

.smallvector:
   lsl     r5, r7, #2
   cmp     r3, r7
   bls     .done
.tailcase:
   add    r5, r5, #1
   vld1.32    d1, [r1]! @ s2, s3 = ar, ai
   vld1.32    d0, [r2]! @ s0, s1 = br, bi
   vmul.f32   s4, s0, s2 @ s4 = ar*br
   vmul.f32   s5, s0, s3 @ s5 = ar*bi
   vmls.f32   s4, s1, s3 @ s4 = s4 - ai*bi
   vmla.f32   s5, s1, s2 @ s5 = s5 + ai*br
   vst1.32    d2, [r0]!
   cmp     r3, r5
   bne     .tailcase 
.done:
   pop     {r4, r5, r6, r7, r8, r9, r15}
