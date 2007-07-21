#
# Copyright 2005,2006 Free Software Foundation, Inc.
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

rbfs = 				\
	rev2/std_2rxhb_2tx.rbf	\
	rev2/std_4rx_0tx.rbf	\
	rev4/std_2rxhb_2tx.rbf	\
	rev4/std_4rx_0tx.rbf    \
	rev2/multi_2rxhb_2tx.rbf	\
	rev4/multi_2rxhb_2tx.rbf		


EXTRA_DIST = 			\
	$(rbfs)


install-data-local:
	@for file in $(rbfs); do \
		echo "$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/$$file"; \
		$(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(datadir)/$$file; \
	done

uninstall-local:
	@for file in $(rbfs); do \
		echo "$(RM) $(DESTDIR)$(datadir)/$$file"; \
		$(RM) $(DESTDIR)$(datadir)/$$file; \
	done
