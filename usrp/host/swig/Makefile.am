#
# Copyright 2001,2003,2004,2006,2007,2008 Free Software Foundation, Inc.
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

ourpythondir = $(usrppythondir)
ourlibdir    = $(usrppyexecdir)


AM_CPPFLAGS = $(USRP_INCLUDES) 		\
	   $(PYTHON_CPPFLAGS) 		\
	   -I$(srcdir)			\
	   $(WITH_INCLUDES)

LOCAL_IFILES =				\
	prims.i


ALL_IFILES =				\
	$(LOCAL_IFILES)			


EXTRA_DIST = 				\
	$(LOCAL_IFILES)


BUILT_SOURCES =				\
	prims.cc			\
	usrp_prims.py


ourpython_PYTHON = 			\
	__init__.py			\
	usrp_fpga_regs.py		\
	usrp_prims.py			


SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(AM_CPPFLAGS) $(WITH_SWIG_INCLUDES) \
	$(WITH_INCLUDES)


ourlib_LTLIBRARIES = 			\
	_usrp_prims.la

_usrp_prims_la_SOURCES = 		\
	prims.cc		


noinst_HEADERS =

_usrp_prims_la_LIBADD  = $(USRP_LA) -lstdc++ $(PYTHON_LDFLAGS)
_usrp_prims_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version


prims.cc usrp_prims.py : prims.i ../../firmware/include/fpga_regs_common.h ../../firmware/include/fpga_regs_standard.h
	$(SWIG) $(SWIGPYTHONARGS) -module usrp_prims -o prims.cc prims.i


MOSTLYCLEANFILES = \
	prims.cc usrp_prims.py *~ *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

