#
# Copyright 2009 Free Software Foundation, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

include $(top_srcdir)/Makefile.common

AM_CPPFLAGS = \
    $(VRT_INCLUDES) \
    $(STD_DEFINES_AND_INCLUDES) \
    $(CPPUNIT_INCLUDES)

LDADD = \
	$(VRT_LA) \
	$(GRUEL_LA)


bin_PROGRAMS = 

noinst_PROGRAMS = \
	simple_rx_samples

simple_rx_samples_SOURCES = simple_rx_samples.cc

