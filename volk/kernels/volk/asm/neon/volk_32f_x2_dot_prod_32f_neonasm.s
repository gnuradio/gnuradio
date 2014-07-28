@ static inline void volk_32f_x2_dot_prod_32f_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_32f_x2_dot_prod_32f_neonasm
volk_32f_x2_dot_prod_32f_neonasm:
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

    veor.32 q0, q0, q0
	movs quarterPoints, num_points, lsr #2
	beq .loop2 @ if zero into quarterPoints

	mov	number, #0	@ number, 0
.loop1:
	pld [aVector, #128] @ pre-load hint - this is implementation specific!
	pld [bVector, #128] @ pre-load hint - this is implementation specific!

	vld1.32	{q1}, [aVector:128]!	@ aVal
	vld1.32	{q2}, [bVector:128]!	@ bVal
    vmla.f32 q0, q1, q2

	add	number, number, #1
	cmp	number, quarterPoints
	ble	.loop1	@ first loop
    
    @ strange order comes from trying to schedule instructions
    vadd.f32 s0, s0, s1
    vadd.f32 s2, s2, s3
	mov	number, quarterPoints, asl #2
    vadd.f32 s0, s0, s2

.loop2:
	cmp	num_points, number
	bls	.done

	vld1.32 {d1[0]}, [aVector]!
	vld1.32 {d1[1]}, [bVector]!
	vmla.f32 s0, s2, s3
	add number, number, #1
	b .loop2

.done:
	vstr s0, [cVector]
	ldmfd	sp!, {r7, r8, sl} @ epilogue - restore register states
	bx	lr
