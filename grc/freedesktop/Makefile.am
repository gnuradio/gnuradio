#
# Copyright 2008 Free Software Foundation, Inc.
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

include $(top_srcdir)/grc/Makefile.inc

ourdatadir = $(grc_data_prefix)/freedesktop
dist_ourdata_DATA = \
	grc-icon-256.png \
	grc-icon-128.png \
	grc-icon-64.png \
	grc-icon-48.png \
	grc-icon-32.png \
	gnuradio-grc.xml \
	gnuradio-grc.desktop \
	gnuradio-usrp_diagnostics.desktop

dist_bin_SCRIPTS = grc_setup_freedesktop
BUILT_SOURCES = grc_setup_freedesktop
grc_setup_freedesktop: Makefile $(srcdir)/grc_setup_freedesktop.in
	sed -e 's|@SRCDIR[@]|$(ourdatadir)|g' $(srcdir)/grc_setup_freedesktop.in > $@

EXTRA_DIST = $(srcdir)/grc_setup_freedesktop.in
MOSTLYCLEANFILES = $(BUILT_SOURCES)

install-data-hook: grc_setup_freedesktop
	grc_setup_freedesktop install
uninstall-hook: grc_setup_freedesktop
	bash $(srcdir)/grc_setup_freedesktop uninstall $(srcdir)
