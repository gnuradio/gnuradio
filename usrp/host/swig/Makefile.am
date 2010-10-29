#
# Copyright 2001,2003,2004,2006,2007,2008,2009,2010 Free Software Foundation, Inc.
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

AM_CPPFLAGS =			\
	$(USRP_INCLUDES)	\
	-I$(srcdir)		\
	$(PYTHON_CPPFLAGS)	\
	$(USB_INCLUDES)		\
	$(WITH_INCLUDES)

#################################
# SWIG interface and library

TOP_SWIG_IFILES =		\
	usrp_prims.i

# Install so that they end up available as:
#   import usrpm.usrp_prims
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/usrpm
usrp_prims_pythondir_category =	\
	usrpm

# additional arguments to the SWIG command
usrp_prims_swig_args =		\
	$(USRP_INCLUDES)

# additional libraries for linking with the SWIG-generated library
usrp_prims_la_swig_libadd =	\
	$(USRP_LA)

# additional Python files to be installed along with the SWIG-generated one
usrp_prims_python =		\
	__init__.py		\
	usrp_fpga_regs.py

include $(top_srcdir)/Makefile.swig

# add some of the variables generated inside the Makefile.swig.gen
BUILT_SOURCES = $(swig_built_sources)

# Do not distribute the output of SWIG
no_dist_files = $(swig_built_sources)
