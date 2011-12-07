#
# Copyright 2004,2005,2006,2007,2008,2009,2010 Free Software Foundation, Inc.
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
include $(top_srcdir)/Makefile.swig

TESTS =

EXTRA_DIST += \
	run_guile_tests.in \
	trellis.test


AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	$(GR_DIGITAL_INCLUDES) \
	$(WITH_INCLUDES)

# ----------------------------------------------------------------
# these scripts generate trellis codes from template files

core_generator = 				\
	generate_all.py				\
	generate_trellis.py			\
	trellis_encoder_XX.cc.t			\
	trellis_encoder_XX.h.t			\
	trellis_encoder_XX.i.t			\
	trellis_sccc_encoder_XX.cc.t		\
	trellis_sccc_encoder_XX.h.t		\
	trellis_sccc_encoder_XX.i.t		\
	trellis_pccc_encoder_XX.cc.t		\
	trellis_pccc_encoder_XX.h.t		\
	trellis_pccc_encoder_XX.i.t		\
	trellis_metrics_X.cc.t			\
	trellis_metrics_X.h.t			\
	trellis_metrics_X.i.t			\
	trellis_viterbi_combined_XX.cc.t	\
	trellis_viterbi_combined_XX.h.t		\
	trellis_viterbi_combined_XX.i.t		\
	trellis_sccc_decoder_combined_XX.cc.t	\
	trellis_sccc_decoder_combined_XX.h.t	\
	trellis_sccc_decoder_combined_XX.i.t	\
	trellis_sccc_decoder_X.cc.t		\
	trellis_sccc_decoder_X.h.t		\
	trellis_sccc_decoder_X.i.t		\
	trellis_pccc_decoder_X.cc.t		\
	trellis_pccc_decoder_X.h.t		\
	trellis_pccc_decoder_X.i.t		\
	trellis_pccc_decoder_combined_XX.cc.t	\
	trellis_pccc_decoder_combined_XX.h.t	\
	trellis_pccc_decoder_combined_XX.i.t	\
	trellis_viterbi_X.cc.t			\
	trellis_viterbi_X.h.t			\
	trellis_viterbi_X.i.t

# Source built by Python into $(builddir)
python_built_sources =		\
	$(GENERATED_H)		\
	$(GENERATED_I)		\
	$(GENERATED_CC)		\
	trellis_generated.i

EXTRA_DIST += 			\
	$(core_generator)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
        trellis_api.h				\
        fsm.h				\
        quicksort_index.h		\
        base.h				\
        interleaver.h			\
        calc_metric.h		\
        core_algorithms.h		\
        trellis_permutation.h		\
        siso_type.h		\
	trellis_siso_f.h		\
	trellis_siso_combined_f.h	\
	trellis_constellation_metrics_cf.h \
	$(GENERATED_H)

lib_LTLIBRARIES = libgnuradio-trellis.la

libgnuradio_trellis_la_SOURCES = 	\
        fsm.cc				\
        quicksort_index.cc		\
        base.cc				\
        interleaver.cc			\
        calc_metric.cc		\
        core_algorithms.cc		\
        trellis_permutation.cc		\
	trellis_siso_f.cc		\
	trellis_siso_combined_f.cc	\
	trellis_constellation_metrics_cf.cc \
	$(GENERATED_CC)

libgnuradio_trellis_la_LIBADD =	\
	$(GNURADIO_CORE_LA)

libgnuradio_trellis_la_LDFLAGS = $(NO_UNDEFINED) $(LTVERSIONFLAGS)

trellis.py trellis.scm:	trellis_generated.i

#################################
# SWIG interface and library

TOP_SWIG_DOC_IFILES = \
	trellis_swig_doc.i

TOP_SWIG_IFILES = 			\
	$(TOP_SWIG_DOC_IFILES) 	\
	trellis.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.trellis
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio
trellis_pythondir_category =		\
	gnuradio

# additional libraries for linking with the SWIG-generated library
trellis_la_swig_libadd =		\
	libgnuradio-trellis.la

# additional SWIG files to be installed
trellis_swiginclude_headers = 		\
	$(GENERATED_I)			\
	fsm.i				\
	interleaver.i			\
	trellis_permutation.i		\
	trellis_siso_f.i		\
	trellis_siso_combined_f.i	\
	trellis_constellation_metrics_cf.i \
	trellis_generated.i		\
	$(TOP_SWIG_DOC_IFILES)

# Do creation and inclusion of other Makefiles last

# include the srcdir's Makefile.gen; doing this creates an implicit
# dependency between $(srcdir)/Makefile.in and $(srcdir)/Makefile.gen.
include $(srcdir)/Makefile.gen

# common way for generating local Makefile.gen
makefile_gen_gen_command = PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) do_makefile=1 do_sources=0 $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.gen.gen

# common way for generating sources from templates when using
# BUILT_SOURCES, using parallel build protection.
gen_sources = $(python_built_sources)
gen_sources_deps = $(core_generator)
par_gen_command = PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_all.py
include $(top_srcdir)/Makefile.par.gen

BUILT_SOURCES += $(python_built_sources)

# Location of non-standard SWIG interface files
trellis_swig_args =    \
	$(GR_DIGITAL_INCLUDES)

if GUILE
TESTS += run_guile_tests
endif
