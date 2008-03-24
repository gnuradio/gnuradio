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

include $(top_srcdir)/Makefile.common

SUBDIRS = spu runtime general procs .

# generate libgcell.la from the convenience libraries in subdirs

lib_LTLIBRARIES = libgcell.la libgcell-qa.la

libgcell_la_SOURCES = 
libgcell_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libgcell_qa_la_SOURCES = 
libgcell_qa_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

libgcell_la_LIBADD = \
	runtime/libruntime.la \
	-lspe2 \
	$(OMNITHREAD_LA)

libgcell_qa_la_LIBADD = \
	runtime/libruntime-qa.la \
	$(CPPUNIT_LIBS)






