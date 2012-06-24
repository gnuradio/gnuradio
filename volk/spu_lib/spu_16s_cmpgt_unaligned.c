#include<spu_intrinsics.h>

void* libvector_16s_cmpgt_unaligned(void* target, void* src, signed short val, unsigned int num_bytes){
	//loop iterator i
	int i = 0;
	void* retval = target;


	//put the target and source addresses into qwords
	vector unsigned int address_counter_tgt = {(unsigned int)target, 0, 0, 0};
	vector unsigned int address_counter_src = {(unsigned int)src, 0, 0 ,0};

	//create shuffle masks

	//shuffle mask building blocks:
	//all from the first vector
	vector unsigned char oneup = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
								  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	//all from the second vector
	vector unsigned char second_oneup = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
										 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};



	//gamma: second half of the second, first half of the first, break at (unsigned int)src%16
	vector unsigned char src_cmp = spu_splats((unsigned char)((unsigned int)src%16));
	vector unsigned char gt_res = spu_cmpgt(oneup, src_cmp);
	vector unsigned char eq_res = spu_cmpeq(oneup, src_cmp);
	vector unsigned char cmp_res = spu_or(gt_res, eq_res);
	vector unsigned char sixteen_uchar = spu_splats((unsigned char)16);
	vector unsigned char phase_change = spu_and(sixteen_uchar, cmp_res);
	vector unsigned int shuffle_mask_gamma = spu_add((vector unsigned int)phase_change,
												 (vector unsigned int)oneup);
	shuffle_mask_gamma = spu_rlqwbyte(shuffle_mask_gamma, (unsigned int)src%16);




	vector unsigned char tgt_second = spu_rlqwbyte(second_oneup, -((unsigned int)target%16));
	vector unsigned char tgt_first = spu_rlqwbyte(oneup, -((unsigned int)target%16));

	//alpha: first half of first, second half of second, break at (unsigned int)target%16
	src_cmp = spu_splats((unsigned char)((unsigned int)target%16));
	gt_res = spu_cmpgt(oneup, src_cmp);
	eq_res = spu_cmpeq(oneup, src_cmp);
	cmp_res = spu_or(gt_res, eq_res);
	phase_change = spu_and(sixteen_uchar, cmp_res);
	vector unsigned int shuffle_mask_alpha = spu_add((vector unsigned int)phase_change,
													 (vector unsigned int)oneup);

	//delta: first half of first, first half of second, break at (unsigned int)target%16
	vector unsigned char shuffle_mask_delta = spu_shuffle(oneup, tgt_second, (vector unsigned char)shuffle_mask_alpha);
	//epsilon: second half of second, second half of first, break at (unsigned int)target%16
	vector unsigned char shuffle_mask_epsilon = spu_shuffle(tgt_second, oneup, (vector unsigned char)shuffle_mask_alpha);
	//zeta: second half of second, first half of first, break at 16 - (unsigned int)target%16
	vector unsigned int shuffle_mask_zeta = spu_rlqwbyte(shuffle_mask_alpha, (unsigned int)target%16);

	//beta: first half of first, second half of second, break at num_bytes%16
	src_cmp = spu_splats((unsigned char)(num_bytes%16));
	gt_res = spu_cmpgt(oneup, src_cmp);
	eq_res = spu_cmpeq(oneup, src_cmp);
	cmp_res = spu_or(gt_res, eq_res);
	phase_change = spu_and(sixteen_uchar, cmp_res);
	vector unsigned int shuffle_mask_beta = spu_add((vector unsigned int)phase_change,
													 (vector unsigned int)oneup);






	qword src_past;
	qword src_present;
	qword tgt_past;
	qword tgt_present;

	qword in_temp;
	qword out_temp0;
	qword out_temp1;

	src_past = si_lqd((qword)address_counter_src, 0);
	tgt_past = si_lqd((qword)address_counter_tgt, 0);

	vector signed short vec_val = spu_splats(val);
	vector unsigned short compare;
	vector unsigned short ones = {1, 1, 1, 1, 1, 1, 1, 1};
	vector unsigned short after_and;

	for(i = 0; i < num_bytes/16; ++i) {

		src_present = si_lqd((qword)address_counter_src, 16);
		tgt_present = si_lqd((qword)address_counter_tgt, 16);

		in_temp = spu_shuffle(src_present, src_past, (vector unsigned char)shuffle_mask_gamma);

		compare = spu_cmpgt((vector signed short) in_temp, vec_val);
		after_and = spu_and(compare, ones);


		out_temp0 = spu_shuffle(tgt_past, (qword)after_and, shuffle_mask_delta);
		out_temp1 = spu_shuffle(tgt_present, (qword)after_and, shuffle_mask_epsilon);

		si_stqd(out_temp0, (qword)address_counter_tgt, 0);
		si_stqd(out_temp1, (qword)address_counter_tgt, 16);

		tgt_past = out_temp1;
		src_past = src_present;
		address_counter_src = spu_add(address_counter_src, 16);
		address_counter_tgt = spu_add(address_counter_tgt, 16);


	}

	src_present = si_lqd((qword)address_counter_src, 16);
	tgt_present = si_lqd((qword)address_counter_tgt, 16);


	in_temp = spu_shuffle(src_present, src_past,(vector unsigned char) shuffle_mask_gamma);

	compare = spu_cmpgt((vector signed short) in_temp, vec_val);
	after_and = spu_and(compare, ones);


	qword target_temp = spu_shuffle(tgt_present, tgt_past, (vector unsigned char) shuffle_mask_zeta);
	qword meld = spu_shuffle((qword)after_and, target_temp, (vector unsigned char)shuffle_mask_beta);



	out_temp0 = spu_shuffle(tgt_past, meld, shuffle_mask_delta);
	out_temp1 = spu_shuffle(tgt_present, meld, shuffle_mask_epsilon);

	si_stqd(out_temp0, (qword)address_counter_tgt, 0);
	si_stqd(out_temp1, (qword)address_counter_tgt, 16);

	return retval;
}



/*
int main(){

	signed short pooh[48];
	signed short bear[48];

	int i = 0;
	for(i = 0; i < 48; i += 2){
		bear[i] = i;
		bear[i + 1] = -i;
	}

	vector_gt_16bit(&pooh[0],&bear[0], 0, 48 * sizeof(signed short));

	for(i = 0; i < 48; ++i) {
		printf("%d, ", pooh[i]);
	}
	printf("\n");
}
*/

