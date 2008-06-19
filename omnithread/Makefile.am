#
# Copyright 2003,2008 Free Software Foundation, Inc.
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

# This is the omnithread package,
#   extracted from the omniORB-4.0.1 distribution

# we should do some configure hacking to determine these on the fly
OMNITHREAD_DEFINES = -DPthreadDraftVersion=10

AM_CPPFLAGS = $(DEFINES) $(OMNITHREAD_DEFINES) $(OMNITHREAD_INCLUDES) \
	 $(WITH_INCLUDES)

# we call it libgromnithread to avoid a collision with libomnithread on Debian
lib_LTLIBRARIES = libgromnithread.la

# At this point we only support the posix and nt pthreads i/f...

if OMNITHREAD_POSIX
libgromnithread_la_SOURCES =		\
	omni_time.cc \
	posix.cc
endif

if OMNITHREAD_NT
libgromnithread_la_SOURCES =		\
	omni_time.cc \
	nt.cc
endif

libgromnithread_la_LDFLAGS = $(NO_UNDEFINED) 

libgromnithread_la_LIBADD =		\
	$(PTHREAD_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnuradio-omnithread.pc

# ... but this code also came with the package

EXTRA_DIST =				\
	gnuradio-omnithread.pc.in	\
	mach.cc				\
	nt.cc				\
	posix.cc			\
	solaris.cc			\
	threaddata.cc			\
	vxWorks.cc			\
	dir.mk

grinclude_HEADERS =			\
	omnithread.h			\
	omni_time.h			\
	ot_mach.h			\
	ot_nt.h				\
	ot_posix.h			\
	ot_pthread_nt.h			\
	ot_solaris.h			\
	ot_VxThread.h			
