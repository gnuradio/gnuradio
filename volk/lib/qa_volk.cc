/*
 * Copyright 2008 Free Software Foundation, Inc.
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

/*
 * This class gathers together all the test cases for the example
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#include <qa_volk.h>
#include <qa_16s_quad_max_star_aligned16.h>
#include <qa_32fc_dot_prod_aligned16.h>
#include <qa_32fc_square_dist_aligned16.h>
#include <qa_32fc_square_dist_scalar_mult_aligned16.h>
#include <qa_32f_sum_of_poly_aligned16.h>
#include <qa_32fc_index_max_aligned16.h>
#include <qa_32f_index_max_aligned16.h>
#include <qa_32fc_conjugate_dot_prod_aligned16.h>
#include <qa_16s_permute_and_scalar_add_aligned16.h>
#include <qa_16s_branch_4_state_8_aligned16.h>
#include <qa_16s_max_star_horizontal_aligned16.h>
#include <qa_16s_max_star_aligned16.h>
#include <qa_16s_add_quad_aligned16.h>
#include <qa_32f_add_aligned16.h>
#include <qa_32f_subtract_aligned16.h>
#include <qa_32f_max_aligned16.h>
#include <qa_32f_min_aligned16.h>
#include <qa_64f_max_aligned16.h>
#include <qa_64f_min_aligned16.h>
#include <qa_32s_and_aligned16.h>
#include <qa_32s_or_aligned16.h>
#include <qa_32f_dot_prod_aligned16.h>
#include <qa_32f_dot_prod_unaligned16.h>
#include <qa_32f_fm_detect_aligned16.h>
#include <qa_32fc_32f_multiply_aligned16.h>
#include <qa_32fc_multiply_aligned16.h>
#include <qa_32f_divide_aligned16.h>
#include <qa_32f_multiply_aligned16.h>
#include <qa_32f_sqrt_aligned16.h>
#include <qa_8sc_multiply_conjugate_16sc_aligned16.h>
#include <qa_8sc_multiply_conjugate_32fc_aligned16.h>
#include <qa_32u_popcnt_aligned16.h>
#include <qa_64u_popcnt_aligned16.h>
#include <qa_16u_byteswap_aligned16.h>
#include <qa_32u_byteswap_aligned16.h>
#include <qa_64u_byteswap_aligned16.h>
#include <qa_32f_normalize_aligned16.h>
#include <qa_16sc_deinterleave_16s_aligned16.h>
#include <qa_16sc_deinterleave_32f_aligned16.h>
#include <qa_16sc_deinterleave_real_16s_aligned16.h>
#include <qa_16sc_deinterleave_real_32f_aligned16.h>
#include <qa_16sc_deinterleave_real_8s_aligned16.h>
#include <qa_16sc_magnitude_16s_aligned16.h>
#include <qa_16sc_magnitude_32f_aligned16.h>
#include <qa_32fc_deinterleave_32f_aligned16.h>
#include <qa_32fc_deinterleave_64f_aligned16.h>
#include <qa_32fc_deinterleave_real_16s_aligned16.h>
#include <qa_32fc_deinterleave_real_32f_aligned16.h>
#include <qa_32fc_deinterleave_real_64f_aligned16.h>
#include <qa_32fc_magnitude_16s_aligned16.h>
#include <qa_32fc_magnitude_32f_aligned16.h>
#include <qa_32f_interleave_16sc_aligned16.h>
#include <qa_32f_interleave_32fc_aligned16.h>
#include <qa_8sc_deinterleave_16s_aligned16.h>
#include <qa_8sc_deinterleave_32f_aligned16.h>
#include <qa_8sc_deinterleave_real_16s_aligned16.h>
#include <qa_8sc_deinterleave_real_32f_aligned16.h>
#include <qa_8sc_deinterleave_real_8s_aligned16.h>
#include <qa_16s_convert_32f_aligned16.h>
#include <qa_16s_convert_32f_unaligned16.h>
#include <qa_16s_convert_8s_aligned16.h>
#include <qa_16s_convert_8s_unaligned16.h>
#include <qa_32f_convert_16s_aligned16.h>
#include <qa_32f_convert_16s_unaligned16.h>
#include <qa_32f_convert_32s_aligned16.h>
#include <qa_32f_convert_32s_unaligned16.h>
#include <qa_32f_convert_64f_aligned16.h>
#include <qa_32f_convert_64f_unaligned16.h>
#include <qa_32f_convert_8s_aligned16.h>
#include <qa_32f_convert_8s_unaligned16.h>
#include <qa_32s_convert_32f_aligned16.h>
#include <qa_32s_convert_32f_unaligned16.h>
#include <qa_64f_convert_32f_aligned16.h>
#include <qa_64f_convert_32f_unaligned16.h>
#include <qa_8s_convert_16s_aligned16.h>
#include <qa_8s_convert_16s_unaligned16.h>
#include <qa_8s_convert_32f_aligned16.h>
#include <qa_8s_convert_32f_unaligned16.h>
#include <qa_32fc_32f_power_32fc_aligned16.h>
#include <qa_32f_power_aligned16.h>
#include <qa_32fc_atan2_32f_aligned16.h>
#include <qa_32fc_power_spectral_density_32f_aligned16.h> 
#include <qa_32fc_power_spectrum_32f_aligned16.h>
#include <qa_32f_calc_spectral_noise_floor_aligned16.h>
#include <qa_32f_accumulator_aligned16.h>
#include <qa_32f_stddev_aligned16.h>
#include <qa_32f_stddev_and_mean_aligned16.h>

CppUnit::TestSuite *
qa_volk::suite()
{
  CppUnit::TestSuite *s = new CppUnit::TestSuite("volk");
  s->addTest(qa_16s_quad_max_star_aligned16::suite());
  s->addTest(qa_32fc_dot_prod_aligned16::suite());
  s->addTest(qa_32fc_square_dist_scalar_mult_aligned16::suite());
  s->addTest(qa_32fc_square_dist_aligned16::suite());
  s->addTest(qa_32f_sum_of_poly_aligned16::suite());
  s->addTest(qa_32fc_index_max_aligned16::suite());
  s->addTest(qa_32f_index_max_aligned16::suite());
  s->addTest(qa_32fc_conjugate_dot_prod_aligned16::suite());
  s->addTest(qa_16s_permute_and_scalar_add_aligned16::suite());
  s->addTest(qa_16s_branch_4_state_8_aligned16::suite());
  s->addTest(qa_16s_max_star_horizontal_aligned16::suite());
  s->addTest(qa_16s_max_star_aligned16::suite());
  s->addTest(qa_16s_add_quad_aligned16::suite());
  s->addTest(qa_32f_add_aligned16::suite());
  s->addTest(qa_32f_subtract_aligned16::suite());
  s->addTest(qa_32f_max_aligned16::suite());
  s->addTest(qa_32f_min_aligned16::suite());
  s->addTest(qa_64f_max_aligned16::suite());
  s->addTest(qa_64f_min_aligned16::suite());
  s->addTest(qa_32s_and_aligned16::suite());
  s->addTest(qa_32s_or_aligned16::suite());
  s->addTest(qa_32f_dot_prod_aligned16::suite());
  s->addTest(qa_32f_dot_prod_unaligned16::suite());
  s->addTest(qa_32f_fm_detect_aligned16::suite());
  s->addTest(qa_32fc_32f_multiply_aligned16::suite());
  s->addTest(qa_32fc_multiply_aligned16::suite());
  s->addTest(qa_32f_divide_aligned16::suite());
  s->addTest(qa_32f_multiply_aligned16::suite());
  s->addTest(qa_32f_sqrt_aligned16::suite());
  s->addTest(qa_8sc_multiply_conjugate_16sc_aligned16::suite());
  s->addTest(qa_8sc_multiply_conjugate_32fc_aligned16::suite());
  s->addTest(qa_32u_popcnt_aligned16::suite());
  s->addTest(qa_64u_popcnt_aligned16::suite());
  s->addTest(qa_16u_byteswap_aligned16::suite());
  s->addTest(qa_32u_byteswap_aligned16::suite());
  s->addTest(qa_64u_byteswap_aligned16::suite());
  s->addTest(qa_32f_normalize_aligned16::suite());
  s->addTest(qa_16sc_deinterleave_16s_aligned16::suite());
  s->addTest(qa_16sc_deinterleave_32f_aligned16::suite());
  s->addTest(qa_16sc_deinterleave_real_16s_aligned16::suite());
  s->addTest(qa_16sc_deinterleave_real_32f_aligned16::suite());
  s->addTest(qa_16sc_deinterleave_real_8s_aligned16::suite());
  s->addTest(qa_16sc_magnitude_16s_aligned16::suite());
  s->addTest(qa_16sc_magnitude_32f_aligned16::suite());
  s->addTest(qa_32fc_deinterleave_32f_aligned16::suite());
  s->addTest(qa_32fc_deinterleave_64f_aligned16::suite());
  s->addTest(qa_32fc_deinterleave_real_16s_aligned16::suite());
  s->addTest(qa_32fc_deinterleave_real_32f_aligned16::suite());
  s->addTest(qa_32fc_deinterleave_real_64f_aligned16::suite());
  s->addTest(qa_32fc_magnitude_16s_aligned16::suite());
  s->addTest(qa_32fc_magnitude_32f_aligned16::suite());
  s->addTest(qa_32f_interleave_16sc_aligned16::suite());
  s->addTest(qa_32f_interleave_32fc_aligned16::suite());
  s->addTest(qa_8sc_deinterleave_16s_aligned16::suite());
  s->addTest(qa_8sc_deinterleave_32f_aligned16::suite());
  s->addTest(qa_8sc_deinterleave_real_16s_aligned16::suite());
  s->addTest(qa_8sc_deinterleave_real_32f_aligned16::suite());
  s->addTest(qa_8sc_deinterleave_real_8s_aligned16::suite());
  s->addTest(qa_16s_convert_32f_aligned16::suite());
  s->addTest(qa_16s_convert_32f_unaligned16::suite());
  s->addTest(qa_16s_convert_8s_aligned16::suite());
  s->addTest(qa_16s_convert_8s_unaligned16::suite());
  s->addTest(qa_32f_convert_16s_aligned16::suite());
  s->addTest(qa_32f_convert_16s_unaligned16::suite());
  s->addTest(qa_32f_convert_32s_aligned16::suite());
  s->addTest(qa_32f_convert_32s_unaligned16::suite());
  s->addTest(qa_32f_convert_64f_aligned16::suite());
  s->addTest(qa_32f_convert_64f_unaligned16::suite());
  s->addTest(qa_32f_convert_8s_aligned16::suite());
  s->addTest(qa_32f_convert_8s_unaligned16::suite());
  s->addTest(qa_32s_convert_32f_aligned16::suite());
  s->addTest(qa_32s_convert_32f_unaligned16::suite());
  s->addTest(qa_64f_convert_32f_aligned16::suite());
  s->addTest(qa_64f_convert_32f_unaligned16::suite());
  s->addTest(qa_8s_convert_16s_aligned16::suite());
  s->addTest(qa_8s_convert_16s_unaligned16::suite());
  s->addTest(qa_8s_convert_32f_aligned16::suite());
  s->addTest(qa_8s_convert_32f_unaligned16::suite());
  s->addTest(qa_32fc_32f_power_32fc_aligned16::suite());
  s->addTest(qa_32f_power_aligned16::suite());
  s->addTest(qa_32fc_atan2_32f_aligned16::suite());
  s->addTest(qa_32fc_power_spectral_density_32f_aligned16::suite());
  s->addTest(qa_32fc_power_spectrum_32f_aligned16::suite());
  s->addTest(qa_32f_calc_spectral_noise_floor_aligned16::suite());
  s->addTest(qa_32f_accumulator_aligned16::suite());
  s->addTest(qa_32f_stddev_aligned16::suite());
  s->addTest(qa_32f_stddev_and_mean_aligned16::suite());

  return s;
}
