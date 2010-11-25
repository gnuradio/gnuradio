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

bin_PROGRAMS = run_waveform

run_waveform_SOURCES = run_waveform.cc
run_waveform_CPPFLAGS = $(GUILE_CFLAGS) -DSRCDIR=\"$(srcdir)\"
run_waveform_LDADD = $(GUILE_LIBS)

DISTCLEANFILES = gr-run-waveform.tar.gz
