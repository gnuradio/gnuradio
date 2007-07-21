#
# Copyright 2007 Free Software Foundation, Inc.
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

datadir = $(prefix)/share/usrp

RBFS = usrp_radar_mono.rbf

EXTRA_DIST = \
	usrp_radar_mono.csf \
	usrp_radar_mono.esf \
	usrp_radar_mono.psf \
	usrp_radar_mono.qpf \
	usrp_radar_mono.qsf \
	usrp_radar_mono.v   \
	$(RBFS)

install-data-local:
	@for file in $(RBFS); do \
		echo "$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/rev2/$$file"; \
		$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/rev2/$$file; \
		echo "$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/rev4/$$file"; \
		$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/rev4/$$file; \
	done

uninstall-local:
	@for file in $(RBFS); do \
		echo "$(RM) $(DESTDIR)$(datadir)/rev2/$$file"; \
		$(RM) $(DESTDIR)$(datadir)/rev2/$$file; \
		echo "$(RM) $(DESTDIR)$(datadir)/rev4/$$file"; \
		$(RM) $(DESTDIR)$(datadir)/rev4/$$file; \
	done


MOSTLYCLEANFILES = 	\
	db/*		\
	*.rpt		\
	*.summary	\
	*.qws		\
	*.smsg		\
	*.done		\
	*.pin		\
	*.sof		\
	*~
