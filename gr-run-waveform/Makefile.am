#
# Copyright 2004,2008,2009 Free Software Foundation, Inc.
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
DIST_SUBDIRS = config
EXTRA_DIST = gen-xyzzy

# build the standalone waveform application
bin_PROGRAMS = run_waveform

# A unit test case for the XYZZY class
check_PROGRAMS = test_xyzzy
test_xyzzy_SOURCES = test_xyzzy.cc xyzzy.cc
test_xyzzy_CPPFLAGS = $(GUILE_CFLAGS) \
	-DSRCDIR=\"$(srcdir)\" \
	-DDATAROOTDIR=\"$(datarootdir)\"
test_xyzzy_LDADD = $(GUILE_LIBS)

# Don't install the internal header
noinst_HEADERS = xyzzy.h

# The standalone waveform application
run_waveform_SOURCES = run_waveform.cc xyzzy.cc
run_waveform_CPPFLAGS = $(GUILE_CFLAGS) \
	-DSRCDIR=\"$(srcdir)\" \
	-DPKGLIBDIR=\"$(pkglibdir)\" \
	-DDATAROOTDIR=\"$(datarootdir)\"
# Uncommenting this force make to statically link in the guile library.
# Note that this will fail to link it you don't have libguile.a.
# run_waveform_LDFLAGS = -static
run_waveform_LDADD = $(GUILE_LIBS)

DISTCLEANFILES = gr-run-waveform.tar.gz
