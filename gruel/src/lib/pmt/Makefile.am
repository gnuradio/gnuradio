#
# Copyright 2008,2009,2010,2011 Free Software Foundation, Inc.
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

AM_CPPFLAGS = $(DEFINES) $(GRUEL_INCLUDES) $(BOOST_CPPFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

noinst_LTLIBRARIES = libpmt.la

# ----------------------------------------------------------------
# these scripts generate code

code_generator = 			\
	generate_unv.py			\
	unv_template.h.t		\
	unv_template.cc.t		\
	unv_qa_template.cc.t		

GENERATED_H =				\
	pmt_unv_int.h			\
	qa_pmt_unv.h			

GENERATED_CC =				\
	pmt_unv.cc			\
	qa_pmt_unv.cc			

python_built_sources = $(GENERATED_H) $(GENERATED_CC)

PMT_SERIAL_TAGS_H = $(abs_top_builddir)/gruel/src/include/gruel/pmt_serial_tags.h
BUILT_SOURCES = $(python_built_sources) $(PMT_SERIAL_TAGS_H)

EXTRA_DIST += $(code_generator)

# ----------------------------------------------------------------

libpmt_la_SOURCES = 			\
	pmt.cc 				\
	pmt_io.cc 			\
	pmt_pool.cc 			\
	pmt_serialize.cc 		\
	pmt_unv.cc

libpmt_la_LIBADD = 			\
	$(BOOST_THREAD_LIB) 		\
	-lstdc++

libpmt_la_LDFLAGS =			\
	$(BOOST_LDFLAGS)

noinst_HEADERS = \
	$(GENERATED_H)			\
	pmt_int.h			\
	qa_pmt.h			\
	qa_pmt_prims.h

# Build the qa code into its own library

noinst_LTLIBRARIES += libpmt-qa.la

libpmt_qa_la_SOURCES =			\
	qa_pmt.cc			\
	qa_pmt_prims.cc			\
	qa_pmt_unv.cc			

# magic flags
libpmt_qa_la_LDFLAGS = $(NO_UNDEFINED) -avoid version

libpmt_qa_la_LIBADD = 			\
	libpmt.la			\
	$(CPPUNIT_LIBS)			\
	-lstdc++			


# Do creation and inclusion of other Makefiles last

# common way for generating sources from templates when using
# BUILT_SOURCES, using parallel build protection.
gen_sources = $(python_built_sources)
gen_sources_deps = $(core_generator)
par_gen_command = PYTHONPATH=$(top_srcdir)/gruel/src/lib/pmt srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_unv.py
include $(top_srcdir)/Makefile.par.gen

# Rule to create the build header file using python
# Doesn't need parallel protections because there is a single target
$(PMT_SERIAL_TAGS_H): $(top_srcdir)/gruel/src/scheme/gnuradio/gen-serial-tags.py $(top_srcdir)/gruel/src/scheme/gnuradio/pmt-serial-tags.scm
	$(PYTHON) $^ $@
