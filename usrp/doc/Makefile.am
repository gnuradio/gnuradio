#
# Copyright 2001,2005,2009 Free Software Foundation, Inc.
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


SUBDIRS = other

man3dir = $(mandir)/man3
usrp_docdir  = $(prefix)/share/doc/usrp-$(DOCVER)

EXTRA_DIST = 			\
	Doxyfile.in		\
	ddc.eps			\
	ddc.png			\
	usrp-block-diagram.eps	\
	usrp-block-diagram.png	\
	usrp.jpg		\
	usrp_guide.xml		

if HAS_XMLTO
DOCBOOK_HTML_FILES=usrp_guide.html
all-local: dox docbook-html
else
DOCBOOK_HTML_FILES=
all-local: dox
endif

dist_usrp_doc_DATA = $(top_srcdir)/usrp/README

dox: html/index.html
html/index.html:
	$(MKDIR_P) html
	@DOXYGEN@

docbook-html: usrp_guide.html

usrp_guide.html: usrp_guide.xml
	xmlto html-nochunks $(top_srcdir)/usrp/doc/usrp_guide.xml

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(usrp_docdir)/html
	@for i in $(DOCBOOK_HTML_FILES); do \
		echo "$(INSTALL_DATA) $$i $(DESTDIR)$(usrp_docdir)/html"; \
		$(INSTALL_DATA) $$i $(DESTDIR)$(usrp_docdir)/html; \
	done
	cp -r html $(DESTDIR)$(usrp_docdir)

uninstall-local:
	$(RM) -fr $(DESTDIR)$(usrp_docdir)/html

clean-local:
	$(RM) -fr latex html man xml $(DOCBOOK_HTML_FILES)
