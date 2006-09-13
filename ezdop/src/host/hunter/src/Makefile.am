# Copyright 2006 Johnathan Corgan.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

bin_PROGRAMS = hunter
hunter_SOURCES = \
                 calibrate.cc \
                 doppler.cc   \
                 gps.cc       \
                 histogram.cc \
                 hunter.cc    \
                 hunterapp.cc \
                 hunter.xrc    \
                 known.cc     \
                 resource.cc  \
                 sample.cc    \
                 samplelog.cc \
                 search.cc    \
                 serial.cc    \
                 settings.cpp \
                 spherical.cc \
                 tactical.cc

# FIXME: put in config macro
EZDOP_LIBS = -L/usr/local/lib -lezdop

hunter_CXXFLAGS = $(WX_FLAGS)
hunter_LDADD = \
    $(WX_LIBS) \
    $(EZDOP_LIBS)

BUILT_SOURCES = \
    $(top_builddir)/src/resource.cc
    
$(top_builddir)/src/resource.cc: hunter.xrc
	wxrc -c -o $(top_builddir)/src/resource.cc $(top_srcdir)/src/hunter.xrc

MOSTLYCLEANFILES = $(BUILT_SOURCES)