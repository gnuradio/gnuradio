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

install-data-local:
	xdg-icon-resource install --context mimetypes --mode system --size 48 $(srcdir)/grc-icon-48.png application-gnuradio-grc
	xdg-icon-resource install --context apps --mode system --size 256 $(srcdir)/grc-icon-256.png gnuradio-grc
	xdg-icon-resource install --context apps --mode system --size 32 $(srcdir)/grc-icon-32.png gnuradio-grc
	xdg-mime install --mode system $(srcdir)/gnuradio-grc.xml
	xdg-desktop-menu install --mode system $(srcdir)/gnuradio-grc.desktop
	xdg-desktop-menu install --mode system $(srcdir)/gnuradio-usrp_diagnostics.desktop

uninstall-local:
	xdg-icon-resource uninstall --context mimetypes --mode system --size 48 application-gnuradio-grc
	xdg-icon-resource uninstall --context apps --mode system --size 256 gnuradio-grc
	xdg-icon-resource uninstall --context apps --mode system --size 32 gnuradio-grc
	xdg-mime uninstall --mode system $(srcdir)/gnuradio-grc.xml
	xdg-desktop-menu uninstall --mode system $(srcdir)/gnuradio-grc.desktop
	xdg-desktop-menu uninstall --mode system $(srcdir)/gnuradio-usrp_diagnostics.desktop

EXTRA_DIST = \
	$(srcdir)/grc-icon-256.png \
	$(srcdir)/grc-icon-48.png \
	$(srcdir)/grc-icon-32.png \
	$(srcdir)/gnuradio-grc.xml \
	$(srcdir)/gnuradio-grc.desktop \
	$(srcdir)/gnuradio-usrp_diagnostics.desktop
