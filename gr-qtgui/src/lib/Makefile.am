#
# Copyright 2004,2005 Free Software Foundation, Inc.
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
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

include $(top_srcdir)/Makefile.common

INCLUDES = $(STD_DEFINES_AND_INCLUDES)	\
	   $(QT_CFLAGS)			\
	   $(QWT_CFLAGS) 

# This rule lets GNU create any moc_*.cc files from the equivalent *.h
moc_%.cc: %.h
	moc $< -o $@

# Generate the .h and .cc files from the .ui file
%.h: %.ui
	uic $< -o $@

%.cc: %.ui
	uic -impl $*.h $< -o $@

include_HEADERS =
	fftdisplay.h			\
	fftdisplaysink.h

noinst_PROGRAMS = 			\
	qt_examples	

BUILT_SOURCES = 			\
	moc_fftdisplay.cc

nodist_qt_examples_SOURCES =		\
	$(BUILT_SOURCES)
	
qt_examples_SOURCES = 			\
	fftdisplay.cc			\
	qt_examples.cc

qt_examples_LDADD = $(QWT_LIBS) 	\
		    $(QT_LIBS)		\
		    $(GNURADIO_CORE_LA)


qt_examples_LDFLAGS = $(QT_CFLAGS) 	\
		      $(QWT_CFLAGS)

MOSTLYCLEANFILES = 			\
	*~ $(BUILT_SOURCES)
