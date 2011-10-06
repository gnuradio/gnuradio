#
# Copyright 2001,2002,2004-2011 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

#
# This directory contains mostly filter routines, plus a few
# other performance critical items
#

# $(WITH_INCLUDES) must _always_ be last
AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) \
	$(FFTW3F_CPPFLAGS) $(WITH_INCLUDES)

noinst_LTLIBRARIES = libfilter.la libfilter-qa.la

# ----------------------------------------------------------------
# these scripts generate FIR code
#

code_generator = 					\
	generate_all.py					\
	generate_gr_fir_XXX.py				\
	generate_gr_fir_filter_XXX.py			\
	generate_gr_interp_fir_filter_XXX.py		\
	generate_gr_rational_resampler_base_XXX.py	\
	generate_gr_fir_sysconfig.py			\
	generate_gr_fir_sysconfig_generic.py		\
	generate_gr_fir_util.py				\
	generate_gr_freq_xlating_fir_filter_XXX.py	\
	generate_gri_fir_filter_with_buffer_XXX.py	\
	generate_utils.py				\
	gr_fir_XXX.cc.t					\
	gr_fir_XXX.h.t					\
	gr_fir_XXX_generic.cc.t				\
	gr_fir_XXX_generic.h.t				\
	gr_fir_filter_XXX.cc.t				\
	gr_fir_filter_XXX.h.t				\
	gr_fir_filter_XXX.i.t				\
	gr_interp_fir_filter_XXX.cc.t			\
	gr_interp_fir_filter_XXX.h.t			\
	gr_interp_fir_filter_XXX.i.t			\
	gr_rational_resampler_base_XXX.cc.t		\
	gr_rational_resampler_base_XXX.h.t		\
	gr_rational_resampler_base_XXX.i.t		\
	gr_freq_xlating_fir_filter_XXX.cc.t		\
	gr_freq_xlating_fir_filter_XXX.h.t		\
	gr_freq_xlating_fir_filter_XXX.i.t		\
	gri_fir_filter_with_buffer_XXX.cc.t		\
	gri_fir_filter_with_buffer_XXX.h.t


# Source built by Python into $(builddir)
BUILT_SOURCES +=		\
	$(GENERATED_H)		\
	$(GENERATED_I)		\
	$(GENERATED_CC)		\
	filter_generated.i

# ----------------------------------------------------------------
# MD_CPU and MD_SUBCPU are set at configure time by way of 
#   gnuradio/config/gr_set_md_cpu.m4.  
#   It indicates which set of machine dependent code we should be building.
#   We currently implement "generic" and "x86"

#
# <foo>_CODE entry for each set of machine specific speedups
#

generic_CODE =				\
	sysconfig_generic.cc		

generic_qa_CODE =			\
	qa_dotprod_generic.cc

x86_CODE = 				\
	sysconfig_x86.cc		\
	gr_fir_sysconfig_x86.cc		\
	gr_cpu_x86.cc			\
	gr_fir_ccc_simd.cc		\
	gr_fir_ccc_x86.cc		\
	gr_fir_fff_simd.cc		\
	gr_fir_fff_x86.cc		\
	gr_fir_fsf_simd.cc		\
	gr_fir_fsf_x86.cc		\
	gr_fir_scc_simd.cc		\
	gr_fir_scc_x86.cc		\
	gr_fir_fcc_simd.cc		\
	gr_fir_fcc_x86.cc		\
	gr_fir_ccf_simd.cc		\
	gr_fir_ccf_x86.cc		\
	sse_debug.c

x86_SUBCODE = 				\
	float_dotprod_sse.S		\
	float_dotprod_3dnow.S		\
	complex_dotprod_3dnowext.S	\
	complex_dotprod_3dnow.S		\
	complex_dotprod_sse.S		\
	ccomplex_dotprod_3dnowext.S	\
	ccomplex_dotprod_3dnow.S	\
	ccomplex_dotprod_sse.S		\
	fcomplex_dotprod_3dnow.S	\
	fcomplex_dotprod_sse.S		\
	short_dotprod_mmx.S

x86_64_SUBCODE = 			\
	float_dotprod_sse64.S		\
	float_dotprod_3dnow64.S		\
	complex_dotprod_3dnowext64.S	\
	complex_dotprod_3dnow64.S	\
	complex_dotprod_sse64.S		\
	ccomplex_dotprod_3dnowext64.S	\
	ccomplex_dotprod_3dnow64.S	\
	ccomplex_dotprod_sse64.S	\
	fcomplex_dotprod_3dnow64.S	\
	fcomplex_dotprod_sse64.S	\
	short_dotprod_mmx64.S

x86_qa_CODE = 				\
	qa_dotprod_x86.cc		\
	qa_float_dotprod_x86.cc		\
	qa_complex_dotprod_x86.cc	\
	qa_ccomplex_dotprod_x86.cc	

powerpc_CODE = \
	sysconfig_powerpc.cc \
	gr_fir_sysconfig_powerpc.cc \
	gr_cpu_powerpc.cc \
	gr_fir_fff_altivec.cc \
	gr_altivec.c \
	dotprod_fff_altivec.c

powerpc_qa_CODE = \
	qa_dotprod_powerpc.cc

armv7_a_CODE = \
	sysconfig_armv7_a.cc \
	gr_fir_sysconfig_armv7_a.cc \
	gr_cpu_armv7_a.cc \
	gr_fir_fff_armv7_a.cc \
	dotprod_fff_armv7_a.c \
	gr_fir_ccf_armv7_a.cc \
	dotprod_ccf_armv7_a.c

armv7_a_qa_CODE = \
	qa_dotprod_armv7_a.cc

#
# include each <foo>_CODE entry here...
#
EXTRA_libfilter_la_SOURCES =		\
	$(generic_CODE)			\
	$(generic_qa_CODE)		\
	$(x86_CODE)			\
	$(x86_SUBCODE)			\
	$(x86_64_SUBCODE)		\
	$(x86_qa_CODE)			\
	$(powerpc_CODE)			\
	$(powerpc_qa_CODE)		\
	$(armv7_a_CODE)		\
	$(armv7_a_qa_CODE)


EXTRA_DIST += \
	3dnow_float_dotprod_really_simple.S	\
	3dnow_float_dotprod_simple.S		\
	$(code_generator)

# work around automake deficiency
libfilter_la_common_SOURCES = 		\
	$(GENERATED_CC)			\
	gr_adaptive_fir_ccc.cc		\
	gr_adaptive_fir_ccf.cc		\
	gri_fft_filter_fff_generic.cc	\
	gri_fft_filter_ccc_generic.cc	\
	gr_fft_filter_ccc.cc		\
	gr_fft_filter_fff.cc		\
	gr_goertzel_fc.cc		\
	gr_filter_delay_fc.cc		\
	gr_fractional_interpolator_ff.cc \
	gr_fractional_interpolator_cc.cc \
	gr_hilbert_fc.cc		\
	gr_iir_filter_ffd.cc		\
	gr_sincos.c			\
	gr_single_pole_iir_filter_ff.cc	\
	gr_single_pole_iir_filter_cc.cc	\
	gri_goertzel.cc			\
	gri_mmse_fir_interpolator.cc	\
	gri_mmse_fir_interpolator_cc.cc	\
	complex_dotprod_generic.cc	\
	ccomplex_dotprod_generic.cc	\
	float_dotprod_generic.c		\
	short_dotprod_generic.c		\
	gr_pfb_channelizer_ccf.cc	\
	gr_pfb_synthesis_filterbank_ccf.cc\
	gr_pfb_decimator_ccf.cc		\
	gr_pfb_interpolator_ccf.cc	\
	gr_pfb_arb_resampler_ccf.cc	\
	gr_pfb_arb_resampler_fff.cc	\
	gr_pfb_clock_sync_ccf.cc	\
	gr_pfb_clock_sync_fff.cc	\
	gr_dc_blocker_cc.cc		\
	gr_dc_blocker_ff.cc

libfilter_qa_la_common_SOURCES = 	\
	qa_filter.cc			\
	qa_gr_fir_ccf.cc		\
	qa_gr_fir_fcc.cc		\
	qa_gr_fir_fff.cc		\
	qa_gr_fir_ccc.cc		\
	qa_gr_fir_scc.cc		\
	qa_gr_rotator.cc		\
	qa_gri_mmse_fir_interpolator.cc	\
	qa_gri_mmse_fir_interpolator_cc.cc \
	qa_gri_fir_filter_with_buffer_ccf.cc \
	qa_gri_fir_filter_with_buffer_ccc.cc \
	qa_gri_fir_filter_with_buffer_fcc.cc \
	qa_gri_fir_filter_with_buffer_fff.cc \
	qa_gri_fir_filter_with_buffer_fsf.cc \
	qa_gri_fir_filter_with_buffer_scc.cc

if MD_CPU_generic
libfilter_la_SOURCES = $(libfilter_la_common_SOURCES) $(generic_CODE)
libfilter_qa_la_SOURCES = $(libfilter_qa_la_common_SOURCES) $(generic_qa_CODE)
endif

if MD_CPU_x86
if MD_SUBCPU_x86_64
libfilter_la_SOURCES = $(libfilter_la_common_SOURCES) $(x86_64_SUBCODE) $(x86_CODE)
else
libfilter_la_SOURCES = $(libfilter_la_common_SOURCES) $(x86_SUBCODE) $(x86_CODE)
endif

libfilter_qa_la_SOURCES = $(libfilter_qa_la_common_SOURCES) $(x86_qa_CODE)
endif

if MD_CPU_powerpc
libfilter_la_SOURCES = $(libfilter_la_common_SOURCES) $(powerpc_CODE)
libfilter_qa_la_SOURCES = $(libfilter_qa_la_common_SOURCES) $(powerpc_qa_CODE)
endif

if MD_CPU_armv7_a
libfilter_la_SOURCES = $(libfilter_la_common_SOURCES) $(armv7_a_CODE)
libfilter_qa_la_SOURCES = $(libfilter_qa_la_common_SOURCES) $(armv7_a_qa_CODE)
endif


grinclude_HEADERS = 			\
	$(GENERATED_H)			\
	complex_dotprod_generic.h	\
	complex_dotprod_x86.h		\
	fcomplex_dotprod_x86.h		\
	ccomplex_dotprod_generic.h	\
	ccomplex_dotprod_x86.h		\
	float_dotprod_generic.h		\
	float_dotprod_x86.h		\
	gr_adaptive_fir_ccc.h		\
	gr_adaptive_fir_ccf.h		\
	gr_altivec.h			\
	gr_cpu.h			\
	gri_fft_filter_fff_generic.h	\
	gri_fft_filter_ccc_generic.h	\
	gr_fft_filter_ccc.h		\
	gr_fft_filter_fff.h		\
	gr_filter_delay_fc.h		\
	gr_fir_sysconfig_x86.h		\
	gr_fir_sysconfig_powerpc.h	\
	gr_fractional_interpolator_ff.h	\
	gr_fractional_interpolator_cc.h	\
	gr_goertzel_fc.h		\
	gr_hilbert_fc.h			\
	gr_iir_filter_ffd.h		\
	gr_rotator.h			\
	gr_sincos.h			\
	gr_single_pole_iir.h		\
	gr_single_pole_iir_filter_ff.h	\
	gr_single_pole_iir_filter_cc.h  \
	gr_vec_types.h			\
	gri_goertzel.h			\
	gri_iir.h			\
	gri_mmse_fir_interpolator.h	\
	gri_mmse_fir_interpolator_cc.h	\
	qa_filter.h			\
	short_dotprod_generic.h		\
	short_dotprod_x86.h		\
	sse_debug.h			\
	gr_pfb_channelizer_ccf.h	\
	gr_pfb_synthesis_filterbank_ccf.h\
	gr_pfb_decimator_ccf.h		\
	gr_pfb_interpolator_ccf.h	\
	gr_pfb_arb_resampler_ccf.h	\
	gr_pfb_arb_resampler_fff.h	\
	gr_pfb_clock_sync_ccf.h		\
	gr_pfb_clock_sync_fff.h		\
	gr_dc_blocker_cc.h		\
	gr_dc_blocker_ff.h

noinst_HEADERS = 			\
	assembly.h			\
	gcc_x86_cpuid.h			\
	dotprod_fff_altivec.h		\
	dotprod_fff_armv7_a.h		\
	dotprod_ccf_armv7_a.h		\
	gr_fir_scc_simd.h		\
	gr_fir_scc_x86.h		\
	gr_fir_fcc_simd.h		\
	gr_fir_fcc_x86.h		\
	gr_fir_ccf_simd.h		\
	gr_fir_ccf_x86.h		\
	gr_fir_ccc_simd.h		\
	gr_fir_ccc_x86.h		\
	gr_fir_fff_altivec.h		\
	gr_fir_fff_armv7_a.h		\
	gr_fir_ccf_armv7_a.h		\
	gr_fir_fff_simd.h		\
	gr_fir_fff_x86.h		\
	gr_fir_fsf_simd.h		\
	gr_fir_fsf_x86.h		\
	interpolator_taps.h		\
	qa_complex_dotprod_x86.h	\
	qa_ccomplex_dotprod_x86.h	\
	qa_dotprod.h			\
	qa_float_dotprod_x86.h		\
	qa_gr_fir_ccf.h			\
	qa_gr_fir_fcc.h			\
	qa_gr_fir_fff.h			\
	qa_gr_fir_ccc.h			\
	qa_gr_fir_scc.h			\
	qa_gr_rotator.h			\
	qa_gri_mmse_fir_interpolator.h	\
	qa_gri_mmse_fir_interpolator_cc.h \
	qa_gri_fir_filter_with_buffer_ccf.h \
	qa_gri_fir_filter_with_buffer_ccc.h \
	qa_gri_fir_filter_with_buffer_fcc.h \
	qa_gri_fir_filter_with_buffer_fff.h \
	qa_gri_fir_filter_with_buffer_fsf.h \
	qa_gri_fir_filter_with_buffer_scc.h


swiginclude_HEADERS =			\
	filter.i			\
	filter_generated.i		\
	gr_adaptive_fir_ccc.i		\
	gr_adaptive_fir_ccf.i		\
	gr_fft_filter_ccc.i		\
	gr_fft_filter_fff.i		\
	gr_filter_delay_fc.i		\
	gr_fractional_interpolator_ff.i \
	gr_fractional_interpolator_cc.i \
	gr_goertzel_fc.i		\
	gr_hilbert_fc.i			\
	gr_iir_filter_ffd.i		\
	gr_single_pole_iir_filter_ff.i	\
	gr_single_pole_iir_filter_cc.i	\
	gr_pfb_channelizer_ccf.i	\
	gr_pfb_synthesis_filterbank_ccf.i\
	gr_pfb_decimator_ccf.i		\
	gr_pfb_interpolator_ccf.i	\
	gr_pfb_arb_resampler_ccf.i	\
	gr_pfb_arb_resampler_fff.i	\
	gr_pfb_clock_sync_ccf.i		\
	gr_pfb_clock_sync_fff.i		\
	gr_dc_blocker_cc.i		\
	gr_dc_blocker_ff.i		\
	$(GENERATED_I)


# Do creation and inclusion of other Makefiles last

# include the srcdir's Makefile.gen; doing this creates an implicit
# dependency between $(srcdir)/Makefile.in and $(srcdir)/Makefile.gen.
include $(srcdir)/Makefile.gen

# common way for generating local Makefile.gen
makefile_gen_gen_command = PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) do_makefile=1 do_sources=0 $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.gen.gen

# common way for generating sources from templates when using
# BUILT_SOURCES, using parallel build protection.
gen_sources = $(BUILT_SOURCES)
gen_sources_deps = $(core_generator)
par_gen_command = PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.par.gen
