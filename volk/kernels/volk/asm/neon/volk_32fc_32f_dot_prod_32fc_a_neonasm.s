@ static inline void volk_32fc_32f_dot_prod_32fc_a_neonasm ( lv_32fc_t* result, const  lv_32fc_t* input, const  float* taps, unsigned int num_points) {
    .global	volk_32fc_32f_dot_prod_32fc_a_neonasm
    volk_32fc_32f_dot_prod_32fc_a_neonasm:
    @ r0 - result: pointer to output array (32fc)
    @ r1 - input: pointer to input array 1 (32fc)
    @ r2 - taps: pointer to input array 2 (32f)
    @ r3 - num_points: number of items to process
    
    result .req r0
    input .req r1
    taps .req r2
    num_points .req r3
    quarterPoints .req r7
    number .req r8
    @ Note that according to the ARM EABI (AAPCS) Section 5.1.1:
    @ registers s16-s31 (d8-d15, q4-q7) must be preserved across subroutine calls;
    @ registers s0-s15 (d0-d7, q0-q3) do not need to be preserved
    @ registers d16-d31 (q8-q15), if present, do not need to be preserved.
    realAccQ   .req q0 @ d0-d1/s0-s3
    compAccQ   .req q1 @ d2-d3/s4-s7
    realAccS   .req s0 @ d0[0]
    compAccS   .req s4 @ d2[0]
    tapsVal    .req q2 @ d4-d5
    outVal     .req q3 @ d6-d7
    realMul    .req q8 @ d8-d9
    compMul    .req q9 @ d16-d17
    inRealVal  .req q10 @ d18-d19
    inCompVal  .req q11 @ d20-d21
    
    stmfd	sp!, {r7, r8, sl}	@ prologue - save register states
    
    veor realAccQ, realAccQ @ zero out accumulators
    veor compAccQ, compAccQ @ zero out accumulators
    movs quarterPoints, num_points, lsr #2
    beq .loop2 @ if zero into quarterPoints
    
    mov number, quarterPoints

.loop1:
    @ do work here
    @pld [taps, #128] @ pre-load hint - this is implementation specific!
    @pld [input, #128] @ pre-load hint - this is implementation specific!
    vld1.32 {d4-d5}, [taps:128]! @ tapsVal
    vld2.32 {d20-d23}, [input:128]! @ inRealVal, inCompVal
    vmul.f32 realMul, tapsVal, inRealVal
    vmul.f32 compMul, tapsVal, inCompVal
    vadd.f32 realAccQ, realAccQ, realMul
    vadd.f32 compAccQ, compAccQ, compMul
    subs number, number, #1
    bne	.loop1	@ first loop

    @ Sum up across realAccQ and compAccQ
    vpadd.f32 d0, d0, d1      @ realAccQ +-> d0
    vpadd.f32 d2, d2, d3      @ compAccQ +-> d2
    vadd.f32 realAccS, s0, s1 @ sum the contents of d0 together (realAccQ)
    vadd.f32 compAccS, s4, s5 @ sum the contents of d2 together (compAccQ)
    @ critical values are now in s0 (realAccS), s4 (realAccQ)
	mov	number, quarterPoints, asl #2

.loop2:
    cmp	num_points, number
    bls	.done
    
    vld1.32 {d4[0]}, [taps]! @ s8
    vld2.32 {d5[0],d6[0]}, [input]! @ s10, s12
    vmul.f32 s5, s8, s10
    vmul.f32 s6, s8, s12
    vadd.f32 realAccS, realAccS, s5
    vadd.f32 compAccS, compAccS, s6
    
    add number, number, #1
    b .loop2

.done:
    vst1.32 {d0[0]}, [result]! @ realAccS
    vst1.32 {d2[0]}, [result]  @ compAccS

    ldmfd	sp!, {r7, r8, sl} @ epilogue - restore register states
    bx	lr
