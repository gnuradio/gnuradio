@ static inline void volk_16i_max_star_horizontal_16i_neonasm(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
	.global	volk_16i_max_star_horizontal_16i_neonasm
volk_16i_max_star_horizontal_16i_neonasm:
	@ r0 - cVector: pointer to output array
	@ r1 - aVector: pointer to input array 1
	@ r2 - num_points: number of items to process

volk_16i_max_star_horizontal_16i_neonasm:
    pld     [r1:128]
    push    {r4, r5, r6}    @ preserve register states
    lsrs    r5, r2, #4      @ 1/16th points = num_points/16
    vmov.i32    q12, #0     @ q12 = [0,0,0,0]
    beq .smallvector        @ less than 16 elements in vector
    mov r4, r1              @ r4 = aVector
    mov r12, r0             @ gcc calls this ip
    mov r3, #0              @ number = 0

.loop1:
    vld2.16 {d16-d19}, [r4]! @ aVector, interleaved load
    pld [r4:128]
    add r3, r3, #1           @ number += 1
    cmp r3, r5               @ number < 1/16th points
    vsub.i16    q10, q8, q9  @ subtraction
    vcge.s16    q11, q10, #0 @ result > 0?
    vcgt.s16    q10, q12, q10 @ result < 0?
    vand.i16    q11, q8, q11 @ multiply by comparisons
    vand.i16    q10, q9, q10 @ multiply by other comparison
    vadd.i16    q10, q11, q10 @ add results to get max
    vst1.16 {d20-d21}, [r12]! @ store the results
    bne .loop1               @ at least 16 items left
    add r1, r1, r3, lsl #5   
    add r0, r0, r3, lsl #4
.smallvector:
    ands    r2, r2, #15
    beq .end
    mov r3, #0
.loop3:
    ldrh    r4, [r1]
    bic r5, r3, #1
    ldrh    ip, [r1, #2]
    add r3, r3, #2
    add r1, r1, #4
    rsb r6, ip, r4
    sxth    r6, r6
    cmp r6, #0
    movgt   ip, r4
    cmp r3, r2
    strh    ip, [r0, r5]
    bcc .loop3
.end:
    pop {r4, r5, r6}
    bx  lr
