#
# Copyright 2001,2005 Free Software Foundation, Inc.
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

SUBDIRS = other xml-swig

docdir  = $(prefix)/share/doc/@PACKAGE@-@VERSION@

EXTRA_DIST = 

all-local: prep @generate_docs@
doc: docs            # alias

docs: prep html/index.html
html/index.html:
	@DOXYGEN@

prep:
	mkdir -p html
	mkdir -p xml

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	@for i in $(top_srcdir)/README $(top_srcdir)/README.hacking; do \
		echo "$(INSTALL_DATA) $$i $(DESTDIR)$(docdir)"; \
		$(INSTALL_DATA) $$i $(DESTDIR)$(docdir); \
	done
	cp -r html $(DESTDIR)$(docdir)
	cp -r xml $(DESTDIR)$(docdir)

uninstall-local:
	@for i in README README.hacking; do \
		echo "$(RM) $(DESTDIR)$(docdir)/$$i;"; \
		$(RM) $(DESTDIR)$(docdir)/$$i; \
	done
	$(RM) -fr $(DESTDIR)$(docdir)/html
	$(RM) -fr $(DESTDIR)$(docdir)/xml

clean-local:
	$(RM) -fr latex html man
