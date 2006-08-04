#
# Copyright 2001 Free Software Foundation, Inc.
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

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES) \
           -I$(top_srcdir)/gr-error-correcting-codes/src/lib/libecc

noinst_LTLIBRARIES = libecc-qa.la

noinst_HEADERS =				\
	qa_encoder_convolutional_ic1_ic1.h	\
	qa_ecc.h

libecc_qa_la_SOURCES = 				\
	qa_encoder_convolutional_ic1_ic1.cc	\
	qa_ecc.cc

# list of programs run by "make check" and "make distcheck"

TESTS = test_all

noinst_PROGRAMS = test_all

LIBECC = ../libecc.la
LIBECCQA = libecc-qa.la $(LIBECC)

test_all_SOURCES = test_all.cc
test_all_LDADD = $(LIBECCQA)		\
	$(CPPUNIT_LIBS)
