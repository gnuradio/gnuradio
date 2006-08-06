#
# Copyright 2001,2003,2004,2006 Free Software Foundation, Inc.
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

# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/usrp_prims

ourpythondir = $(pythondir)
ourlibdir    = $(pyexecdir)

INCLUDES = $(USRP_INCLUDES) 		\
	   $(PYTHON_CPPFLAGS) 		\
	   -I$(srcdir)

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


SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(INCLUDES)


ourlib_LTLIBRARIES = 			\
	_usrp_prims.la

_usrp_prims_la_SOURCES = 		\
	prims.cc		


noinst_HEADERS =

_usrp_prims_la_LIBADD  = $(top_builddir)/usrp/host/lib/libusrp.la -lstdc++ $(PYTHON_LDFLAGS)
_usrp_prims_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version


prims.cc usrp_prims.py : prims.i ../../firmware/include/fpga_regs_common.h ../../firmware/include/fpga_regs_standard.h
	$(SWIG) $(SWIGPYTHONARGS) -module usrp_prims -o prims.cc $<


MOSTLYCLEANFILES = \
	prims.cc usrp_prims.py *~ *.pyc

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done

