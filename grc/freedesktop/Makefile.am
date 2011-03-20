#
# Copyright 2008-2011 Free Software Foundation, Inc.
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

ourdatadir = $(pkgdatadir)/grc/freedesktop
dist_ourdata_DATA = \
	grc-icon-256.png \
	grc-icon-128.png \
	grc-icon-64.png \
	grc-icon-48.png \
	grc-icon-32.png \
	gnuradio-grc.xml \
	gnuradio-grc.desktop

pkglibexecdir = $(libexecdir)/$(PACKAGE)
dist_pkglibexec_SCRIPTS = grc_setup_freedesktop

grc_setup_freedesktop: $(srcdir)/grc_setup_freedesktop.in Makefile
	sed -e 's|@SRCDIR[@]|$(ourdatadir)|g' $< > $@

EXTRA_DIST += $(srcdir)/grc_setup_freedesktop.in
BUILT_SOURCES += grc_setup_freedesktop

if XDG_UTILS
install-data-hook:
	@printf "\n*** GRC Post-Install Message ***\
	\nTo install icons, mime type, and menu items\
	\nfor a freedesktop.org system (Gnome/KDE/Xfce):\
	\n  >>> sudo $(pkglibexecdir)/grc_setup_freedesktop install\n\n"

uninstall-hook:
	@printf "\n*** GRC Post-Uninstall Message ***\
	\nTo uninstall icons, mime type, and menu items\
	\nfor a freedesktop.org system (Gnome/KDE/Xfce):\
	\n  >>> sudo $(pkglibexecdir)/grc_setup_freedesktop uninstall\n\n"
endif
