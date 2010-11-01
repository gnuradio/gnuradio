#
# Copyright 2001,2003,2004,2005,2006,2007,2008,2009,2010 Free Software Foundation, Inc.
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

BUILT_SOURCES = $(grinclude_HEADERS) $(swig_built_sources)

CLEANFILES = python/*.cc python/*.h
if GUILE
CLEANFILES += guile/*.cc gnuradio/*.scm
endif

# ----------------------------------------------------------------
# We've split the previously monstrous gnuradio_core into 6
# smaller pieces.  This reduces compile time coupling and creates
# smaller pieces for the compiler to digest.  prior to this change, on
# X86_64, g++'s resident set size was 650MB!
# ----------------------------------------------------------------

TOP_SWIG_IFILES =		\
	gnuradio_core_runtime.i	\
	gnuradio_core_general.i	\
	gnuradio_core_gengen.i	\
	gnuradio_core_filter.i	\
	gnuradio_core_io.i	\
	gnuradio_core_hier.i


# SWIG headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS =			\
	gnuradio.i			\
	gr_swig_block_magic.i		\
	gr_shared_ptr.i

# SWIG headers that get installed in ${prefix}/include/gnuradio/swig/...
nobase_swiginclude_HEADERS = \
	guile/std_complex.i


if GUILE
# This is the top level guile file, which loads all the other scm files
# for gnuradio. This has to be installed top level to be found in the
# default search path.
nobase_guile_DATA = \
	gnuradio/core.scm \
	gnuradio/export-safely.scm \
	gnuradio/runtime-shim.scm \
	Swig/common.scm
endif


if PYTHON
AM_CPPFLAGS = -I$(srcdir) $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
	 $(WITH_INCLUDES)

EXTRA_DIST = gen-swig-bug-fix

# special install for this top-level Python script which includes all
# of the split Python libraries.
ourpythondir = $(grpythondir)/gr
ourpython_PYTHON = gnuradio_core.py

# ----------------------------------------------------------------
# FIXME As of swig 1.3.31, this still seems to be required...

gnuradio_swig_bug_workaround.h : gnuradio_core_runtime.py $(srcdir)/gen-swig-bug-fix
	$(PYTHON) $(srcdir)/gen-swig-bug-fix python/gnuradio_core_runtime.cc $@

# C/C++ headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS = gnuradio_swig_bug_workaround.h

# Install so that they end up available as:
#   import gnuradio.gr
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/gr
gnuradio_core_runtime_pythondir_category = gnuradio/gr
gnuradio_core_general_pythondir_category = gnuradio/gr
gnuradio_core_gengen_pythondir_category  = gnuradio/gr
gnuradio_core_filter_pythondir_category  = gnuradio/gr
gnuradio_core_io_pythondir_category      = gnuradio/gr
gnuradio_core_hier_pythondir_category    = gnuradio/gr

# additional libraries for linking with each SWIG-generated library
gnuradio_core_runtime_la_swig_libadd = $(GNURADIO_CORE_LA)
gnuradio_core_general_la_swig_libadd = $(GNURADIO_CORE_LA)
gnuradio_core_gengen_la_swig_libadd  = $(GNURADIO_CORE_LA)
gnuradio_core_filter_la_swig_libadd  = $(GNURADIO_CORE_LA)
gnuradio_core_io_la_swig_libadd      = $(GNURADIO_CORE_LA)
gnuradio_core_hier_la_swig_libadd    = $(GNURADIO_CORE_LA)

# add some of the variables generated inside the Makefile.swig

# include the SWIG-generated .h files in the BUILT SOURCES, since they
# aren't by default when using Makefile.swig; order doesn't matter.
PYTHON_GEN = $(foreach HFILE,$(TOP_SWIG_IFILES), $(subst .i,.py,$(HFILE)))
swig_built_sources += $(PYTHON_GEN)
endif				# end of if python

if GUILE
#GUILE_GEN = $(foreach HFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,%.scm,$(HFILE)))
GUILE_GEN = $(foreach HFILE,$(TOP_SWIG_IFILES), $(patsubst %.i,gnuradio/%.scm,$(HFILE)))
# GUILE_GEN_STAMPS = $(filter %.scm,$(TOP_SWIG_IFILES))
swig_built_sources += $(GUILE_GEN)
endif

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)

