#
# Copyright 2006,2008 Free Software Foundation, Inc.
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

INCLUDES = $(DEFINES) $(OMNITHREAD_INCLUDES) $(BOOST_CFLAGS) \
	$(CPPUNIT_INCLUDES) $(WITH_INCLUDES)

TESTS = test_pmt

lib_LTLIBRARIES = libpmt.la libpmt-qa.la

# ----------------------------------------------------------------
# these scripts generate code

CODE_GENERATOR = 			\
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


$(GENERATED_H) $(GENERATED_I) $(GENERATED_CC): $(CODE_GENERATOR)
	PYTHONPATH=$(top_srcdir)/pmt/src/lib srcdir=$(srcdir) $(PYTHON) $(srcdir)/generate_unv.py

pmt_serial_tags.h: $(srcdir)/../scheme/gnuradio/gen-serial-tags.scm $(srcdir)/../scheme/gnuradio/pmt-serial-tags.scm
	$(RUN_GUILE) $(srcdir)/../scheme/gnuradio/gen-serial-tags.scm $(srcdir)/../scheme/gnuradio/pmt-serial-tags.scm pmt_serial_tags.h

BUILT_SOURCES = $(GENERATED_H) $(GENERATED_CC) pmt_serial_tags.h

# ----------------------------------------------------------------

EXTRA_DIST = 				\
	$(CODE_GENERATOR)		

# These are the source files that go into the pmt shared library
libpmt_la_SOURCES =			\
	pmt.cc				\
	pmt_io.cc			\
	pmt_pool.cc			\
	pmt_serialize.cc		\
	pmt_unv.cc			

# magic flags
libpmt_la_LDFLAGS = $(NO_UNDEFINED)

# link the library against the c++ standard library
libpmt_la_LIBADD = 			\
	$(OMNITHREAD_LA)		\
	-lstdc++			

include_HEADERS =			\
	pmt.h				\
	pmt_pool.h			\
	pmt_serial_tags.h		

noinst_HEADERS = 			\
	$(GENERATED_H)			\
	pmt_int.h			\
	qa_pmt.h			\
	qa_pmt_prims.h			


# Build the qa code into its own library

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


noinst_PROGRAMS	=			\
	test_pmt


LIBPMT   = libpmt.la
LIBPMTQA = libpmt-qa.la $(LIBPMT)

test_pmt_SOURCES = test_pmt.cc
test_pmt_LDADD   = $(LIBPMTQA)


CLEANFILES = $(BUILT_SOURCES) *.pyc
