#
# Copyright 2010,2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

include $(top_srcdir)/Makefile.common

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_CPPFLAGS) \
	$(LV_CXXFLAGS) $(WITH_INCLUDES)

volkincludedir = $(prefix)/include/volk

BUILT_SOURCES: \
	volk_config.h \
	volk_tables.h

volkinclude_HEADERS = \
	volk_complex.h \
	volk_common.h \
	volk_config_fixed.h \
	volk_runtime.h \
	volk_config.h \
	volk_tables.h \
	volk_typedefs.h \
	volk_registry.h \
	volk.h \
	volk_cpu.h \
	volk_environment_init.h \
	volk_16i_x5_add_quad_16i_x4_a16.h \
	volk_16i_branch_4_state_8_a16.h \
	volk_16ic_deinterleave_16i_x2_a16.h \
	volk_16ic_s32f_deinterleave_32f_x2_a16.h \
	volk_16ic_deinterleave_real_16i_a16.h \
	volk_16ic_s32f_deinterleave_real_32f_a16.h \
	volk_16ic_deinterleave_real_8i_a16.h \
	volk_16ic_magnitude_16i_a16.h \
	volk_16ic_s32f_magnitude_32f_a16.h \
	volk_16i_s32f_convert_32f_a16.h \
	volk_16i_s32f_convert_32f_u.h \
	volk_16i_convert_8i_a16.h \
	volk_16i_convert_8i_u.h \
	volk_16i_max_star_16i_a16.h \
	volk_16i_max_star_horizontal_16i_a16.h \
	volk_16i_permute_and_scalar_add_a16.h \
	volk_16i_x4_quad_max_star_16i_a16.h \
	volk_16u_byteswap_a16.h \
	volk_32f_accumulator_s32f_a16.h \
	volk_32f_x2_add_32f_a16.h \
	volk_32fc_32f_multiply_32fc_a16.h \
	volk_32fc_32f_power_32fc_a16.h \
	volk_32f_calc_spectral_noise_floor_a16.h \
	volk_32fc_s32f_atan2_32f_a16.h \
	volk_32fc_x2_conjugate_dot_prod_32fc_a16.h \
	volk_32fc_deinterleave_32f_x2_a16.h \
	volk_32fc_deinterleave_64f_x2_a16.h \
	volk_32fc_deinterleave_real_16i_a16.h \
	volk_32fc_deinterleave_real_32f_a16.h \
	volk_32fc_deinterleave_real_64f_a16.h \
	volk_32fc_x2_dot_prod_32fc_a16.h \
	volk_32fc_index_max_16u_a16.h \
	volk_32fc_s32f_magnitude_16i_a16.h \
	volk_32fc_magnitude_32f_a16.h \
	volk_32fc_x2_multiply_32fc_a16.h \
	volk_32f_s32f_convert_16i_a16.h \
	volk_32f_s32f_convert_16i_u.h \
	volk_32f_s32f_convert_32i_a16.h \
	volk_32f_s32f_convert_32i_u.h \
	volk_32f_convert_64f_a16.h \
	volk_32f_convert_64f_u.h \
	volk_32f_s32f_convert_8i_a16.h \
	volk_32f_s32f_convert_8i_u.h \
	volk_32fc_s32f_x2_power_spectral_density_32f_a16.h \
	volk_32fc_s32f_power_spectrum_32f_a16.h \
	volk_32fc_x2_square_dist_32f_a16.h \
	volk_32fc_x2_s32f_square_dist_scalar_mult_32f_a16.h \
	volk_32f_x2_divide_32f_a16.h \
	volk_32f_x2_dot_prod_32f_a16.h \
	volk_32f_x2_dot_prod_32f_u.h \
	volk_32f_s32f_32f_fm_detect_32f_a16.h \
	volk_32f_index_max_16u_a16.h \
	volk_32f_x2_s32f_interleave_16ic_a16.h \
	volk_32f_x2_interleave_32fc_a16.h \
	volk_32f_x2_max_32f_a16.h \
	volk_32f_x2_min_32f_a16.h \
	volk_32f_x2_multiply_32f_a16.h \
	volk_32f_s32f_normalize_a16.h \
	volk_32f_s32f_power_32f_a16.h \
	volk_32f_sqrt_32f_a16.h \
	volk_32f_s32f_stddev_32f_a16.h \
	volk_32f_stddev_and_mean_32f_x2_a16.h \
	volk_32f_x2_subtract_32f_a16.h \
	volk_32f_x3_sum_of_poly_32f_a16.h \
	volk_32i_x2_and_32i_a16.h \
	volk_32i_s32f_convert_32f_a16.h \
	volk_32i_s32f_convert_32f_u.h \
	volk_32i_x2_or_32i_a16.h \
	volk_32u_byteswap_a16.h \
	volk_32u_popcnt_a16.h \
	volk_64f_convert_32f_a16.h \
	volk_64f_convert_32f_u.h \
	volk_64f_x2_max_64f_a16.h \
	volk_64f_x2_min_64f_a16.h \
	volk_64u_byteswap_a16.h \
	volk_64u_popcnt_a16.h \
	volk_8ic_deinterleave_16i_x2_a16.h \
	volk_8ic_s32f_deinterleave_32f_x2_a16.h \
	volk_8ic_deinterleave_real_16i_a16.h \
	volk_8ic_s32f_deinterleave_real_32f_a16.h \
	volk_8ic_deinterleave_real_8i_a16.h \
	volk_8ic_x2_multiply_conjugate_16ic_a16.h \
	volk_8ic_x2_s32f_multiply_conjugate_32fc_a16.h \
	volk_8i_convert_16i_a16.h \
	volk_8i_convert_16i_u.h \
	volk_8i_s32f_convert_32f_a16.h \
	volk_8i_s32f_convert_32f_u.h 

VOLK_MKTABLES_SOURCES = \
	$(platform_CODE) \
	$(top_srcdir)/lib/volk_rank_archs.c \
	$(top_srcdir)/lib/volk_mktables.c


volk_mktables$(EXEEXT): $(VOLK_MKTABLES_SOURCES)
	$(CC) -o $@ $^ $(AM_CPPFLAGS) -I$(top_builddir)/include

volk_tables.h: volk_mktables$(EXEEXT)
	./volk_mktables$(EXEEXT)

volk_config.h: $(top_builddir)/volk_config.h 
	cp $^ $(top_builddir)/include/volk/$@

distclean-local: 
	rm -f volk_config_fixed.h 
	rm -f volk_config.h
	rm -f volk_cpu.h
	rm -f volk.h
	rm -f volk_registry.h
	rm -f volk_runtime.h
	rm -f volk_typedefs.h
	rm -f volk_tables.h
	rm -f *.pyc
	rm -f Makefile.in
	rm -f volk_environment_init.h
	rm -f volk_mktables
	rm -f $(BUILT_SOURCES)
