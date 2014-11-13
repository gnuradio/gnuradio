/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "qa_utils.h"
#include <volk/volk.h>
#include <boost/test/unit_test.hpp>

//VOLK_RUN_TESTS(volk_16i_x5_add_quad_16i_x4, 1e-4, 2046, 10000);
//VOLK_RUN_TESTS(volk_16i_branch_4_state_8, 1e-4, 2046, 10000);
VOLK_RUN_TESTS(volk_16ic_s32f_deinterleave_real_32f, 1e-5, 32768.0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_deinterleave_real_8i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_deinterleave_16i_x2, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_s32f_deinterleave_32f_x2, 1e-4, 32768.0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_deinterleave_real_16i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_magnitude_16i, 1, 0, 20462, 1);
VOLK_RUN_TESTS(volk_16ic_s32f_magnitude_32f, 1e-5, 32768.0, 20462, 1);
VOLK_RUN_TESTS(volk_16i_s32f_convert_32f, 1e-4, 32768.0, 20462, 1);
VOLK_RUN_TESTS(volk_16i_convert_8i, 0, 0, 20462, 1);
//VOLK_RUN_TESTS(volk_16i_max_star_16i, 0, 0, 20462, 10000);
//VOLK_RUN_TESTS(volk_16i_max_star_horizontal_16i, 0, 0, 20462, 10000);
//VOLK_RUN_TESTS(volk_16i_permute_and_scalar_add, 1e-4, 0, 2046, 1000);
//VOLK_RUN_TESTS(volk_16i_x4_quad_max_star_16i, 1e-4, 0, 2046, 1000);
VOLK_RUN_TESTS(volk_16u_byteswap, 0, 0, 20462, 1);
//VOLK_RUN_TESTS(volk_16i_32fc_dot_prod_32fc, 1e-4, 0, 204602, 1);
VOLK_RUN_TESTS(volk_32f_accumulator_s32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_add_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_32f_multiply_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_log2_32f, 1.5e-1, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_expfast_32f, 1e-1, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_pow_32f, 1e-2, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_sin_32f, 1e-6, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_cos_32f, 1e-6, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_tan_32f, 1e-6, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_atan_32f, 1e-3, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_asin_32f, 1e-3, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_acos_32f, 1e-3, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32f_power_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_calc_spectral_noise_floor_32f, 1e-4, 20.0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32f_atan2_32f, 1e-4, 10.0, 20462, 1);
//VOLK_RUN_TESTS(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 2046, 10000);
VOLK_RUN_TESTS(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_deinterleave_32f_x2, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_deinterleave_64f_x2, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32f_deinterleave_real_16i, 0, 32768, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_deinterleave_real_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_deinterleave_imag_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_deinterleave_real_64f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_x2_dot_prod_32fc, 1e-4, 0, 204603, 1);
VOLK_RUN_TESTS(volk_32fc_32f_dot_prod_32fc, 1e-4, 0, 204602, 1);
VOLK_RUN_TESTS(volk_32fc_index_max_16u, 3, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32f_magnitude_16i, 1, 32768, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_magnitude_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_convert_16i, 1, 32768, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_convert_32i, 1, 1<<31, 20462, 1);
VOLK_RUN_TESTS(volk_32f_convert_64f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_convert_8i, 1, 128, 20462, 1);
//VOLK_RUN_TESTS(volk_32fc_s32f_x2_power_spectral_density_32f, 1e-4, 2046, 10000);
VOLK_RUN_TESTS(volk_32fc_s32f_power_spectrum_32f, 1e-4, 0, 2046, 1);
VOLK_RUN_TESTS(volk_32fc_x2_square_dist_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_x2_s32f_square_dist_scalar_mult_32f, 1e-4, 10, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_divide_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_dot_prod_32f, 1e-4, 0, 204602, 1);
VOLK_RUN_TESTS(volk_32f_x2_dot_prod_16i, 1e-4, 0, 204602, 1);
//VOLK_RUN_TESTS(volk_32f_s32f_32f_fm_detect_32f, 1e-4, 2046, 10000);
VOLK_RUN_TESTS(volk_32f_index_max_16u, 3, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_s32f_interleave_16ic, 1, 32767, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_interleave_32fc, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_max_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_min_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_normalize, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_power_32f, 1e-4, 4, 20462, 1);
VOLK_RUN_TESTS(volk_32f_sqrt_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_stddev_32f, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_32f_stddev_and_mean_32f_x2, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_subtract_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x3_sum_of_poly_32f, 1e-2, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32i_x2_and_32i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32i_s32f_convert_32f, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_32i_x2_or_32i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32u_byteswap, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32u_popcntpuppet_32u, 0, 0, 2046, 10000);
VOLK_RUN_TESTS(volk_64f_convert_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_64f_x2_max_64f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_64f_x2_min_64f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_64u_byteswap, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_64u_popcntpuppet_64u, 0, 0, 2046, 10000);
VOLK_RUN_TESTS(volk_8ic_deinterleave_16i_x2, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_s32f_deinterleave_32f_x2, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_deinterleave_real_16i, 0, 256, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_s32f_deinterleave_real_32f, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_deinterleave_real_8i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_x2_multiply_conjugate_16ic, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_8ic_x2_s32f_multiply_conjugate_32fc, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_8i_convert_16i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_8i_s32f_convert_32f, 1e-4, 100, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_x2_multiply_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_x2_multiply_conjugate_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_conjugate_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_x2_multiply_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32fc_multiply_32fc, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_s32f_multiply_32f, 1e-4, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32fc_s32fc_rotatorpuppet_32fc, 1e-3, (lv_32fc_t)lv_cmake(0.953939201, 0.3), 20462, 1);
VOLK_RUN_TESTS(volk_8u_conv_k7_r2puppet_8u, 0, 0, 2060, 1);
VOLK_RUN_TESTS(volk_32f_invsqrt_32f, 1e-2, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_binary_slicer_32i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_binary_slicer_8i, 0, 0, 20462, 1);
VOLK_RUN_TESTS(volk_32f_tanh_32f, 1e-6, 0, 20462, 1);
