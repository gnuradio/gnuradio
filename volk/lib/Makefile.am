#
# Copyright 2010 Free Software Foundation, Inc.
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

#FIXME: forcing the top_builddir for distcheck seems like a bit
# of a hack. Figure out the right way to do this to find built
# volk_config.h and volk_tables.h

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_CPPFLAGS) \
	-I$(top_builddir)/include \
	$(LV_CXXFLAGS) $(WITH_INCLUDES)


# We build 2 libraries and 1 executable here.  One library contains
# everything except the libcppunit QA code, and one contains only the
# libcppunit-based QA code.  The C++ QA code is especially recommended
# when you have general purpose C or C++ code that may not get
# thoroughly exercised by building and running a GR block.  The
# executable runs the QA code at "make check" time.
#
# N.B., If there's a SWIG generated shared library and associated
# python code, it will be contained in ../python, not here.  (That
# code is conditionally built depending on the state of the
# --without-python configure option.)  However, the .i should be here
# next to the .h that it's based on.


# list of programs run by "make check" and "make distcheck"
#TESTS = test_all
#orc stuff gets built in the ORC directory conditional to ORC being enabled.
#it gets linked in during the build of libvolk as an added library.
#there might be a better way to do this.

lib_LTLIBRARIES = \
	libvolk.la \
	libvolk_runtime.la \
	libvolk_qa.la

EXTRA_DIST = \
	volk_mktables.c		\
	volk_rank_archs.h 	\
	volk_proccpu_sim.c

# ----------------------------------------------------------------
#                      The main library
# ----------------------------------------------------------------

universal_runtime_CODE = 	\
	volk_runtime.c	\
	volk_init.c \
	volk_rank_archs.c	

universal_CODE = 		\
	volk.c 			\
	volk_environment_init.c

generic_CODE = 		\
	volk_cpu_generic.c

x86_CODE = 		\
	volk_cpu_x86.c

x86_SUBCODE = 		\
	cpuid_x86.S

x86_64_SUBCODE = 	\
	cpuid_x86_64.S

powerpc_CODE = \
	volk_cpu_powerpc.c


if MD_CPU_generic
libvolk_la_SOURCES =	\
	$(generic_CODE)		\
	$(universal_CODE)
libvolk_runtime_la_SOURCES =	\
	$(generic_CODE)		\
	$(universal_runtime_CODE)

endif

if MD_CPU_x86
if MD_SUBCPU_x86_64
libvolk_la_SOURCES =	\
	$(x86_CODE)		\
	$(x86_64_SUBCODE)		\
	$(universal_CODE) 		

libvolk_runtime_la_SOURCES =	\
	$(x86_CODE)		\
	$(x86_64_SUBCODE)		\
	$(universal_runtime_CODE) 		
else
libvolk_la_SOURCES =	\
	$(x86_CODE)		\
	$(x86_SUBCODE)	\
	$(universal_CODE)

libvolk_runtime_la_SOURCES =	\
	$(x86_CODE)		\
	$(x86_SUBCODE)	\
	$(universal_runtime_CODE)
endif
endif


if MD_CPU_powerpc
libvolk_la_SOURCES =	\
	$(powerpc_CODE)		\
	$(universal_CODE)

libvolk_runtime_la_SOURCES =	\
	$(powerpc_CODE)		\
	$(universal_runtime_CODE)
endif

volk_orc_LDFLAGS = \
	$(ORC_LDFLAGS) \
	-lorc-0.4
	
volk_orc_LIBADD = \
	../orc/libvolk_orc.la

if HAVE_ORC
libvolk_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 $(volk_orc_LDFLAGS)
libvolk_runtime_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0 $(volk_orc_LDFLAGS)
libvolk_la_LIBADD = $(volk_orc_LIBADD)
else
libvolk_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0
libvolk_runtime_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0
libvolk_la_LIBADD =
endif


# ----------------------------------------------------------------
#        The QA library.  Note libvolk.la in LIBADD
# ----------------------------------------------------------------
libvolk_qa_la_SOURCES = \
	qa_volk.cc \
	qa_16s_quad_max_star_aligned16.cc \
	qa_32fc_dot_prod_aligned16.cc \
	qa_32fc_square_dist_aligned16.cc \
	qa_32fc_square_dist_scalar_mult_aligned16.cc \
	qa_32f_sum_of_poly_aligned16.cc \
	qa_32fc_index_max_aligned16.cc \
	qa_32f_index_max_aligned16.cc \
	qa_32fc_conjugate_dot_prod_aligned16.cc \
	qa_16s_permute_and_scalar_add_aligned16.cc \
	qa_16s_branch_4_state_8_aligned16.cc \
	qa_16s_max_star_horizontal_aligned16.cc \
	qa_16s_max_star_aligned16.cc \
	qa_16s_add_quad_aligned16.cc \
	qa_32f_add_aligned16.cc \
	qa_32f_subtract_aligned16.cc \
	qa_32f_max_aligned16.cc \
	qa_32f_min_aligned16.cc \
	qa_64f_max_aligned16.cc \
	qa_64f_min_aligned16.cc \
	qa_32s_and_aligned16.cc \
	qa_32s_or_aligned16.cc \
	qa_32f_dot_prod_aligned16.cc \
	qa_32f_dot_prod_unaligned16.cc \
	qa_32f_fm_detect_aligned16.cc \
	qa_32fc_32f_multiply_aligned16.cc \
	qa_32fc_multiply_aligned16.cc \
	qa_32f_divide_aligned16.cc \
	qa_32f_multiply_aligned16.cc \
	qa_32f_sqrt_aligned16.cc \
	qa_8sc_multiply_conjugate_16sc_aligned16.cc \
	qa_8sc_multiply_conjugate_32fc_aligned16.cc \
	qa_32u_popcnt_aligned16.cc \
	qa_64u_popcnt_aligned16.cc \
	qa_64u_byteswap_aligned16.cc \
	qa_8sc_deinterleave_32f_aligned16.cc \
	qa_16sc_deinterleave_32f_aligned16.cc \
	qa_8sc_deinterleave_16s_aligned16.cc \
	qa_32f_interleave_32fc_aligned16.cc \
	qa_16u_byteswap_aligned16.cc \
	qa_16sc_deinterleave_16s_aligned16.cc \
	qa_32fc_deinterleave_real_32f_aligned16.cc \
	qa_32fc_magnitude_32f_aligned16.cc \
	qa_32fc_deinterleave_real_64f_aligned16.cc \
	qa_32fc_deinterleave_real_16s_aligned16.cc \
	qa_32fc_magnitude_16s_aligned16.cc \
	qa_32fc_deinterleave_32f_aligned16.cc \
	qa_8sc_deinterleave_real_8s_aligned16.cc \
	qa_32fc_deinterleave_64f_aligned16.cc \
	qa_32f_interleave_16sc_aligned16.cc \
	qa_16sc_deinterleave_real_8s_aligned16.cc \
	qa_16sc_deinterleave_real_32f_aligned16.cc \
	qa_16sc_magnitude_32f_aligned16.cc \
	qa_32u_byteswap_aligned16.cc \
	qa_16sc_deinterleave_real_16s_aligned16.cc \
	qa_8sc_deinterleave_real_32f_aligned16.cc \
	qa_16sc_magnitude_16s_aligned16.cc \
	qa_32f_normalize_aligned16.cc \
	qa_8sc_deinterleave_real_16s_aligned16.cc \
	qa_16s_convert_32f_aligned16.cc \
	qa_16s_convert_32f_unaligned16.cc \
	qa_16s_convert_8s_aligned16.cc \
	qa_16s_convert_8s_unaligned16.cc \
	qa_32f_convert_16s_aligned16.cc \
	qa_32f_convert_16s_unaligned16.cc \
	qa_32f_convert_32s_aligned16.cc \
	qa_32f_convert_32s_unaligned16.cc \
	qa_32f_convert_64f_aligned16.cc \
	qa_32f_convert_64f_unaligned16.cc \
	qa_32f_convert_8s_aligned16.cc \
	qa_32f_convert_8s_unaligned16.cc \
	qa_32s_convert_32f_aligned16.cc \
	qa_32s_convert_32f_unaligned16.cc \
	qa_64f_convert_32f_aligned16.cc \
	qa_64f_convert_32f_unaligned16.cc \
	qa_8s_convert_16s_aligned16.cc \
	qa_8s_convert_16s_unaligned16.cc \
	qa_8s_convert_32f_aligned16.cc \
	qa_8s_convert_32f_unaligned16.cc \
	qa_32fc_32f_power_32fc_aligned16.cc \
	qa_32f_power_aligned16.cc \
	qa_32fc_atan2_32f_aligned16.cc \
	qa_32fc_power_spectral_density_32f_aligned16.cc \
	qa_32fc_power_spectrum_32f_aligned16.cc \
	qa_32f_calc_spectral_noise_floor_aligned16.cc \
	qa_32f_accumulator_aligned16.cc \
	qa_32f_stddev_aligned16.cc \
	qa_32f_stddev_and_mean_aligned16.cc

libvolk_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libvolk_qa_la_LIBADD = \
	libvolk.la \
	libvolk_runtime.la \
	$(CPPUNIT_LIBS)

# ----------------------------------------------------------------
# headers that don't get installed
# ----------------------------------------------------------------
noinst_HEADERS = \
	volk_init.h \
	qa_volk.h \
	assembly.h \
	qa_16s_quad_max_star_aligned16.h \
	qa_32fc_dot_prod_aligned16.h \
	qa_32fc_square_dist_aligned16.h \
	qa_32fc_square_dist_scalar_mult_aligned16.h \
	qa_32f_sum_of_poly_aligned16.h \
	qa_32fc_index_max_aligned16.h \
	qa_32f_index_max_aligned16.h \
	qa_32fc_conjugate_dot_prod_aligned16.h \
	qa_16s_permute_and_scalar_add_aligned16.h \
	qa_16s_branch_4_state_8_aligned16.h \
	qa_16s_max_star_horizontal_aligned16.h \
	qa_16s_max_star_aligned16.h \
	qa_16s_add_quad_aligned16.h \
	qa_32f_add_aligned16.h \
	qa_32f_subtract_aligned16.h \
	qa_32f_max_aligned16.h \
	qa_32f_min_aligned16.h \
	qa_64f_max_aligned16.h \
	qa_64f_min_aligned16.h \
	qa_32s_and_aligned16.h \
	qa_32s_or_aligned16.h \
	qa_32f_dot_prod_aligned16.h \
	qa_32f_dot_prod_unaligned16.h \
	qa_32f_fm_detect_aligned16.h \
	qa_32fc_32f_multiply_aligned16.h \
	qa_32fc_multiply_aligned16.h \
	qa_32f_divide_aligned16.h \
	qa_32f_multiply_aligned16.h \
	qa_32f_sqrt_aligned16.h \
	qa_8sc_multiply_conjugate_16sc_aligned16.h \
	qa_8sc_multiply_conjugate_32fc_aligned16.h \
	qa_32u_popcnt_aligned16.h \
	qa_64u_popcnt_aligned16.h \
	qa_64u_byteswap_aligned16.h \
	qa_8sc_deinterleave_32f_aligned16.h \
	qa_16sc_deinterleave_32f_aligned16.h \
	qa_8sc_deinterleave_16s_aligned16.h \
	qa_32f_interleave_32fc_aligned16.h \
	qa_16u_byteswap_aligned16.h \
	qa_16sc_deinterleave_16s_aligned16.h \
	qa_32fc_deinterleave_real_32f_aligned16.h \
	qa_32fc_magnitude_32f_aligned16.h \
	qa_32fc_deinterleave_real_64f_aligned16.h \
	qa_32fc_deinterleave_real_16s_aligned16.h \
	qa_32fc_magnitude_16s_aligned16.h \
	qa_32fc_deinterleave_32f_aligned16.h \
	qa_8sc_deinterleave_real_8s_aligned16.h \
	qa_32fc_deinterleave_64f_aligned16.h \
	qa_32f_interleave_16sc_aligned16.h \
	qa_16sc_deinterleave_real_8s_aligned16.h \
	qa_16sc_deinterleave_real_32f_aligned16.h \
	qa_16sc_magnitude_32f_aligned16.h \
	qa_32u_byteswap_aligned16.h \
	qa_16sc_deinterleave_real_16s_aligned16.h \
	qa_8sc_deinterleave_real_32f_aligned16.h \
	qa_16sc_magnitude_16s_aligned16.h \
	qa_32f_normalize_aligned16.h \
	qa_8sc_deinterleave_real_16s_aligned16.h \
	qa_16s_convert_32f_aligned16.h \
	qa_16s_convert_32f_unaligned16.h \
	qa_16s_convert_8s_aligned16.h \
	qa_16s_convert_8s_unaligned16.h \
	qa_32f_convert_16s_aligned16.h \
	qa_32f_convert_16s_unaligned16.h \
	qa_32f_convert_32s_aligned16.h \
	qa_32f_convert_32s_unaligned16.h \
	qa_32f_convert_64f_aligned16.h \
	qa_32f_convert_64f_unaligned16.h \
	qa_32f_convert_8s_aligned16.h \
	qa_32f_convert_8s_unaligned16.h \
	qa_32s_convert_32f_aligned16.h \
	qa_32s_convert_32f_unaligned16.h \
	qa_64f_convert_32f_aligned16.h \
	qa_64f_convert_32f_unaligned16.h \
	qa_8s_convert_16s_aligned16.h \
	qa_8s_convert_16s_unaligned16.h \
	qa_8s_convert_32f_aligned16.h \
	qa_8s_convert_32f_unaligned16.h \
	qa_32fc_32f_power_32fc_aligned16.h \
	qa_32f_power_aligned16.h \
	qa_32fc_atan2_32f_aligned16.h \
	qa_32fc_power_spectral_density_32f_aligned16.h \
	qa_32fc_power_spectrum_32f_aligned16.h \
	qa_32f_calc_spectral_noise_floor_aligned16.h \
	qa_32f_accumulator_aligned16.h \
	qa_32f_stddev_aligned16.h \
	qa_32f_stddev_and_mean_aligned16.h


# ----------------------------------------------------------------
# Our test program
# ----------------------------------------------------------------
noinst_PROGRAMS = \
	test_all

test_all_SOURCES = test_all.cc
test_all_LDADD   = libvolk_qa.la


distclean-local: 
	rm -f volk.c
	rm -f volk_cpu_generic.c
	rm -f volk_cpu_powerpc.c
	rm -f volk_cpu_x86.c
	rm -f volk_init.c
	rm -f volk_init.h
	rm -f volk_mktables.c
	rm -f volk_proccpu_sim.c
	rm -f volk_runtime.c
	rm -f volk_tables.h
	rm -f volk_environment_init.c
#SUBDIRS = 

#ifdef BUILD_SSE
#SUBDIRS += sse
#elif BUILD_SPU
#SUBDIRS += spu
#else
#SUBDIRS += port
#endif


