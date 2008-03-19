#
# Copyright 2004,2005,2006,2007,2008 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.trellis module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio

ourpythondir = $(grpythondir)
ourlibdir    = $(grpyexecdir)

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	$(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

# ----------------------------------------------------------------
# these scripts generate trellis codes from template files

CODE_GENERATOR = 				\
	generate_all.py				\
	generate_trellis.py			\
	trellis_encoder_XX.cc.t			\
	trellis_encoder_XX.h.t			\
	trellis_encoder_XX.i.t			\
	trellis_metrics_X.cc.t			\
	trellis_metrics_X.h.t			\
	trellis_metrics_X.i.t			\
	trellis_viterbi_combined_XX.cc.t		\
	trellis_viterbi_combined_XX.h.t		\
	trellis_viterbi_combined_XX.i.t		\
	trellis_viterbi_X.cc.t			\
	trellis_viterbi_X.h.t			\
	trellis_viterbi_X.i.t

include Makefile.gen


# Ensure parallel make does the right thing.
# http://sources.redhat.com/automake/automake.html#Multiple-Outputs

STAMPS = generate-stamp

generate-stamp: $(CODE_GENERATOR)
	@rm -f generate-tmp
	@touch generate-tmp
	PYTHONPATH=$(top_srcdir)/gnuradio-core/src/python srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_all.py
	@mv -f generate-tmp $@

$(GENERATED_H) $(GENERATED_I) $(GENERATED_CC) trellis_generated.i : generate-stamp
## Recover from the removal of $@
	@if test -f $@; then :; else \
               trap 'rm -rf generate-lock generate-stamp' 1 2 13 15; \
               if mkdir generate-lock 2>/dev/null; then \
## This code is being executed by the first process.
                 rm -f generate-stamp; \
                 $(MAKE) $(AM_MAKEFLAGS) generate-stamp; \
                 rmdir generate-lock; \
               else \
## This code is being executed by the follower processes.
## Wait until the first process is done.
                 while test -d generate-lock; do sleep 1; done; \
## Succeed if and only if the first process succeeded.
                 test -f generate-stamp; exit $$?; \
               fi; \
             fi


# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the _trellis shared library
# and knows how to call our extensions.

swig_built_sources = 		\
	trellis.cc		\
	trellis.py				

BUILT_SOURCES = \
	$(GENERATED_H)		\
	$(GENERATED_I)		\
	$(GENERATED_CC)		\
	$(swig_built_sources)	

ALL_IFILES = 			\
	$(LOCAL_IFILES)		\
	$(NON_LOCAL_IFILES)	

NON_LOCAL_IFILES =		\
	$(GNURADIO_I)	

LOCAL_IFILES = 			\
	$(top_srcdir)/gr-trellis/src/lib/trellis.i			

EXTRA_DIST = 			\
	$(CODE_GENERATOR)	\
	$(STAMPS)


# This gets trellis.py installed in the right place
ourpython_PYTHON =		\
	trellis.py

ourlib_LTLIBRARIES = _trellis.la

# These are the source files that go into the shared library
_trellis_la_SOURCES = 			\
	trellis.cc			\
        fsm.cc				\
        quicksort_index.cc		\
        base.cc				\
        interleaver.cc			\
        trellis_calc_metric.cc		\
        trellis_permutation.cc		\
	trellis_siso_f.cc		\
	trellis_siso_combined_f.cc	\
	$(GENERATED_CC)                 

# magic flags
_trellis_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version

# link the library against some comon swig runtime code and the 
# c++ standard library
_trellis_la_LIBADD = 			\
	$(PYTHON_LDFLAGS)		\
	$(GNURADIO_CORE_LA)		\
	-lstdc++

trellis.cc trellis.py: $(ALL_IFILES) $(grinclude_HEADERS)
	$(SWIG) $(SWIGPYTHONARGS) -module trellis -o trellis.cc $(LOCAL_IFILES)

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
        fsm.h				\
        quicksort_index.h		\
        base.h				\
        interleaver.h			\
        trellis_metric_type.h		\
        trellis_calc_metric.h		\
        trellis_permutation.h		\
        trellis_siso_type.h		\
	trellis_siso_f.h		\
	trellis_siso_combined_f.h	\
	$(GENERATED_H)

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)			\
	$(GENERATED_I)			\
	fsm.i				\
	interleaver.i			\
	trellis_permutation.i		\
	trellis_siso_f.i		\
	trellis_siso_combined_f.i	\
	trellis_generated.i

CLEANFILES = $(BUILT_SOURCES) *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(swig_built_sources); do echo $(RM) $(distdir)/$$file; done
	@for file in $(swig_built_sources); do $(RM) $(distdir)/$$file; done
