#include<spu_intrinsics.h>

void* libvector_16s_vector_sum_unaligned(void* target,  void* src0, void* src1, unsigned int num_bytes){
	//loop iterator i
	int i = 0;
	void* retval = target;


	//put the target and source addresses into qwords
	vector unsigned int address_counter_tgt = {(unsigned int)target, 0, 0, 0};
	vector unsigned int address_counter_src0 = {(unsigned int)src0, 0, 0 ,0};
	vector unsigned int address_counter_src1 = {(unsigned int)src1, 0, 0, 0};

	//create shuffle masks

	//shuffle mask building blocks:
	//all from the first vector
	vector unsigned char oneup = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
								  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	//all from the second vector
	vector unsigned char second_oneup = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
										 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};



	//gamma: second half of the second, first half of the first, break at (unsigned int)src0%16
	vector unsigned char src_cmp = spu_splats((unsigned char)((unsigned int)src0%16));
	vector unsigned char gt_res = spu_cmpgt(oneup, src_cmp);
	vector unsigned char eq_res = spu_cmpeq(oneup, src_cmp);
	vector unsigned char cmp_res = spu_or(gt_res, eq_res);
	vector unsigned char sixteen_uchar = spu_splats((unsigned char)16);
	vector unsigned char phase_change = spu_and(sixteen_uchar, cmp_res);
	vector unsigned int shuffle_mask_gamma = spu_add((vector unsigned int)phase_change,
												 (vector unsigned int)oneup);
	shuffle_mask_gamma = spu_rlqwbyte(shuffle_mask_gamma, (unsigned int)src0%16);

	//eta: second half of the second, first half of the first, break at (unsigned int)src1%16
	src_cmp = spu_splats((unsigned char)((unsigned int)src1%16));
	gt_res = spu_cmpgt(oneup, src_cmp);
	eq_res = spu_cmpeq(oneup, src_cmp);
	cmp_res = spu_or(gt_res, eq_res);
	sixteen_uchar = spu_splats((unsigned char)16);
	phase_change = spu_and(sixteen_uchar, cmp_res);
	vector unsigned int shuffle_mask_eta = spu_add((vector unsigned int)phase_change,
												 (vector unsigned int)oneup);
	shuffle_mask_eta = spu_rlqwbyte(shuffle_mask_eta, (unsigned int)src1%16);





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






	qword src0_past;
	qword src0_present;
	qword src1_past;
	qword src1_present;
	qword tgt_past;
	qword tgt_present;

	qword in_temp0;
	qword in_temp1;
	qword out_temp0;
	qword out_temp1;

    vector signed int sum;

	src0_past = si_lqd((qword)address_counter_src0, 0);
	src1_past = si_lqd((qword)address_counter_src1, 0);
	tgt_past = si_lqd((qword)address_counter_tgt, 0);

	for(i = 0; i < num_bytes/16; ++i) {

		src0_present = si_lqd((qword)address_counter_src0, 16);
		src1_present = si_lqd((qword)address_counter_src1, 16);
		tgt_present = si_lqd((qword)address_counter_tgt, 16);

		in_temp0 = spu_shuffle(src0_present, src0_past, (vector unsigned char)shuffle_mask_gamma);
		in_temp1 = spu_shuffle(src1_present, src1_past, (vector unsigned char)shuffle_mask_eta);

		sum = spu_add((vector signed int)in_temp0, (vector signed int)in_temp1);


		out_temp0 = spu_shuffle(tgt_past, (qword)sum, shuffle_mask_delta);
		out_temp1 = spu_shuffle(tgt_present, (qword)sum, shuffle_mask_epsilon);

		si_stqd(out_temp0, (qword)address_counter_tgt, 0);
		si_stqd(out_temp1, (qword)address_counter_tgt, 16);

		tgt_past = out_temp1;
		src0_past = src0_present;
		src1_past = src1_present;
		address_counter_src0 = spu_add(address_counter_src0, 16);
		address_counter_src1 = spu_add(address_counter_src1, 16);
		address_counter_tgt = spu_add(address_counter_tgt, 16);


	}

	src0_present = si_lqd((qword)address_counter_src0, 16);
	src1_present = si_lqd((qword)address_counter_src1, 16);
	tgt_present = si_lqd((qword)address_counter_tgt, 16);


	in_temp0 = spu_shuffle(src0_present, src0_past, (vector unsigned char) shuffle_mask_gamma);
	in_temp1 = spu_shuffle(src1_present, src1_past, (vector unsigned char) shuffle_mask_eta);
	sum = spu_add((vector signed int)in_temp0, (vector signed int)in_temp1);
	qword target_temp = spu_shuffle(tgt_present, tgt_past, (vector unsigned char) shuffle_mask_zeta);
	qword meld = spu_shuffle((qword)sum, target_temp, (vector unsigned char)shuffle_mask_beta);



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
	signed short res[48];

	int i = 0;
	for(i = 0; i < 48; ++i){
		pooh[i] = i;
	}
	for(i = 48; i < 96; ++i){
		bear[i - 48] = i;
	}

	vector_sum(&pooh[9], &pooh[9], &bear[3], 30);

	for(i = 0; i < 48; ++i) {
		printf("%d, ", pooh[i]);
	}
	printf("\n");
}
*/

