#
# Copyright 2008 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(ORC_CFLAGS)

include $(top_srcdir)/Makefile.common
noinst_LTLIBRARIES = libvolk_orc.la
libvolk_orc_la_LDFLAGS = $(ORC_LDFLAGS)

libvolk_orc_la_SOURCES = \
volk_8i_convert_16i_a_orc_impl.orc \
volk_8i_s32f_convert_32f_a_orc_impl.orc \
volk_16u_byteswap_a_orc_impl.orc \
volk_32i_x2_and_32i_a_orc_impl.orc \
volk_32i_x2_or_32i_a_orc_impl.orc \
volk_32f_x2_add_32f_a_orc_impl.orc \
volk_32f_x2_subtract_32f_a_orc_impl.orc \
volk_32f_x2_divide_32f_a_orc_impl.orc \
volk_32f_x2_multiply_32f_a_orc_impl.orc \
volk_32fc_x2_multiply_32fc_a_orc_impl.orc \
volk_32fc_32f_multiply_32fc_a_orc_impl.orc \
volk_32f_sqrt_32f_a_orc_impl.orc \
volk_32f_x2_max_32f_a_orc_impl.orc \
volk_32f_x2_min_32f_a_orc_impl.orc \
volk_32f_s32f_normalize_a_orc_impl.orc \
volk_32fc_magnitude_32f_a_orc_impl.orc \
volk_32fc_s32f_magnitude_16i_a_orc_impl.orc \
volk_16ic_magnitude_16i_a_orc_impl.orc \
volk_16ic_deinterleave_16i_x2_a_orc_impl.orc \
volk_16i_s32f_deinterleave_32f_x2_a_orc_impl.orc \
volk_16ic_deinterleave_real_8i_a_orc_impl.orc	\
volk_32fc_s32fc_multiply_32fc_a_orc_impl.orc	\
volk_32f_s32f_multiply_32f_a_orc_impl.orc



my_ORCC_FLAGS = --implementation $(ORCC_FLAGS)

.orc.c:
	$(ORCC) $(my_ORCC_FLAGS) -o $@ $<
