#
# Copyright 2003 Free Software Foundation, Inc.
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

# This is the omnithread package,
#   extracted from the omniORB-4.0.1 distribution

# we should do some configure hacking to determine these on the fly
OMNITHREAD_DEFINES = -DPthreadDraftVersion=10

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(OMNITHREAD_DEFINES)

noinst_LTLIBRARIES = libomnithread.la

# At this point we only support the posix and nt pthreads i/f...

if OMNITHREAD_POSIX
libomnithread_la_SOURCES =		\
	posix.cc
endif

if OMNITHREAD_NT
libomnithread_la_SOURCES =		\
	nt.cc
endif

libomnithread_la_LIBADD =		\
	$(PTHREAD_LIBS)

# ... but this code also came with the package

EXTRA_DIST =				\
	mach.cc				\
	nt.cc				\
	posix.cc			\
	solaris.cc			\
	threaddata.cc			\
	vxWorks.cc			\
	dir.mk

grinclude_HEADERS =			\
	omnithread.h			\
	ot_mach.h			\
	ot_nt.h				\
	ot_posix.h			\
	ot_pthread_nt.h			\
	ot_solaris.h			\
	ot_VxThread.h			
