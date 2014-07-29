@ static inline void volk_32f_s32f_multiply_32f_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_32f_s32f_multiply_32f_neonasm
volk_32f_s32f_multiply_32f_neonasm:
	@ r0 - cVector: pointer to output array
	@ r1 - aVector: pointer to input array 1
	@ r2 - bVector: pointer to input array 2
	@ r3 - num_points: number of items to process

	stmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, r12}	@ prologue - save register states


    @ quarter_points = num_points / 4
	movs r11, r3, lsr #2
	beq .loop2 @ if zero into quarterPoints

    @ number = quarter_points
	mov r10, r3
    @ copy address of input vector
    mov r4, r1
    @ copy address of output vector
    mov r5, r0

    @ load the scalar to a quad register
    @ vmov.32 d2[0], r2
    @ The scalar might be in s0, not totally sure
    vdup.32 q2, d0[0]

    @ this is giving fits. Current theory is hf has something to do with it
    .loop1:
    @  vld1.32 {q1}, [r4:128]! @ aVal
    @  vmul.f32 q3, q1, q2
    @  vst1.32	{q3}, [r5:128]! @ cVal
    @
    @  subs r10, r10, #1
    @  bne	.loop1	@ first loop

    @ number = quarter_points * 4
    mov	r10, r11, asl #2

    .loop2:
    @   cmp	num_points, number
    @   bls	.done
    @
    @   vld1.32 {d0[0]}, [aVector]!
    @   vmul.f32 s2, s0, s4
    @   vst1.32 {d1[0]}, [cVector]!
    @   add number, number, #1
    @   b .loop2

.done:
	ldmfd	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, r12} @ epilogue - restore register states
	bx	lr
