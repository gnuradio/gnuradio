#
# Copyright 2004,2008,2010 Free Software Foundation, Inc.
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

ACLOCAL_AMFLAGS = -I config

include $(top_srcdir)/Makefile.common

EXTRA_DIST = bootstrap configure config.h.in
SUBDIRS = config
if LV_HAVE_ORC
SUBDIRS += orc
endif
SUBDIRS += include lib

#if USE_PYTHON
#SUBDIRS += python 
#endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = volk.pc

distclean-local:
	-rm -rf autom4te.cache
	-rm -f config.*
	-rm -f depcomp
	-rm -f install-sh
	-rm -f ltmain.sh
	-rm -f py-compile
	-rm -f missing
	-rm -f volk_config.h.in
	-rm -f aclocal.m4
	-rm -f Makefile.in
	-rm -f config/lt*
	-rm -f config/libtool*
	-rm -f apps/Makefile.in
	-rm -f config/Makefile.in
	-rm -f data/Makefile.in
	-rm -f doc/Makefile.in
	-rm -f include/Makefile.in
	-rm -f lib/Makefile.in
	-rm -f python/Makefile.in
	-rm -f configure
	-rm -f orc/Makefile.in
	-rm -f orc/*.c
	-rm -rf gen/config
	-rm -rf gen/include
	-rm -rf gen/lib
