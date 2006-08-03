#
# Copyright 2006 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
# 

include $(top_srcdir)/Makefile.common

TESTS = test_pmt

lib_LTLIBRARIES = libpmt.la libpmt-qa.la

# ----------------------------------------------------------------
# these scripts generate code

CODE_GENERATOR = 			\
	generate_unv.py			\
	unv_template.h.t		\
	unv_template.cc.t		

GENERATED_H =				\
	pmt_unv_int.h			

GENERATED_CC =				\
	pmt_unv.cc			


$(GENERATED_H) $(GENERATED_I) $(GENERATED_CC): $(CODE_GENERATOR)
	PYTHONPATH=$(top_srcdir)/src/lib srcdir=$(srcdir) $(srcdir)/generate_unv.py

BUILT_SOURCES = $(GENERATED_H) $(GENERATED_CC)

# ----------------------------------------------------------------

EXTRA_DIST = 				\
	$(CODE_GENERATOR)


# These are the source files that go into the pmt shared library
libpmt_la_SOURCES =			\
	pmt.cc				\
	$(GENERATED_CC)			

# magic flags
libpmt_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link the library against the c++ standard library
libpmt_la_LIBADD = 			\
	-lstdc++			

include_HEADERS =			\
	pmt.h

noinst_HEADERS = 			\
	$(GENERATED_H)			\
	pmt_int.h			\
	qa_pmt.h			\
	qa_pmt_prims.h			


# Build the qa code into its own library

libpmt_qa_la_SOURCES =			\
	qa_pmt.cc			\
	qa_pmt_prims.cc			

# magic flags
libpmt_qa_la_LDFLAGS = $(NO_UNDEFINED) -avoid-version

# link the library against the c++ standard library
libpmt_qa_la_LIBADD = 			\
	$(CPPUNIT_LIBS)			\
	-lstdc++			


noinst_PROGRAMS	=			\
	test_pmt


LIBPMT   = libpmt.la
LIBPMTQA = libpmt-qa.la $(LIBPMT)

test_pmt_SOURCES = test_pmt.cc
test_pmt_LDADD   = $(LIBPMTQA)


CLEANFILES = $(BUILT_SOURCES) *.pyc
