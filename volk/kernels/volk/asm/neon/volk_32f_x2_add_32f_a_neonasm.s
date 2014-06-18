@ static inline void volk_32f_x2_add_32f_a_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_32f_x2_add_32f_a_neonasm
volk_32f_x2_add_32f_a_neonasm:
	@ r0 - cVector: pointer to output array
	@ r1 - aVector: pointer to input array 1
	@ r2 - bVector: pointer to input array 2
	@ r3 - num_points: number of items to process
	cVector .req r0
	aVector .req r1
	bVector .req r2
	num_points .req r3
	quarterPoints .req r7
	number .req r8
	aVal .req q0 @ d0-d1
	bVal .req q1 @ d2-d3
	cVal .req q2 @ d4-d5

	@ AAPCS Section 5.1.1
	@ A subroutine must preserve the contents of the registers r4-r8, r10, r11 and SP
	stmfd	sp!, {r7, r8, sl}	@ prologue - save register states

	movs quarterPoints, num_points, lsr #2
	beq .loop2 @ if zero into quarterPoints

	mov	number, #0	@ number, 0
.loop1:
	pld [aVector, #128] @ pre-load hint - this is implementation specific!
	pld [bVector, #128] @ pre-load hint - this is implementation specific!

	vld1.32	{d0-d1}, [aVector:128]!	@ aVal
	add	number, number, #1
	vld1.32	{d2-d3}, [bVector:128]!	@ bVal
	vadd.f32 cVal, bVal, aVal
	cmp	number, quarterPoints
	vst1.32	{d4-d5}, [cVector:128]!	@ cVal

	ble	.loop1	@ first loop

	mov	number, quarterPoints, asl #2

.loop2:
	cmp	num_points, number
	bls	.done

	vld1.32 {d0[0]}, [aVector]!
	vld1.32 {d0[1]}, [bVector]!
	vadd.f32 s2, s1, s0
	vst1.32 {d1[0]}, [cVector]!
	add number, number, #1
	b .loop2

.done:
	ldmfd	sp!, {r7, r8, sl} @ epilogue - restore register states
	bx	lr
