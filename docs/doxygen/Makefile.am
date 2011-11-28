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

dist_gr_doc_DATA =			\
	$(top_srcdir)/README		\
	$(top_srcdir)/README.hacking	\
	README.doxyxml

EXTRA_DIST = doxyxml swig_doc.py

all-local: prep @generate_docs@
doc: docs            # alias

docs: prep html/index.html
html/index.html:
	@DOXYGEN@

EXTRA_DIST = images/gnuradio-logo.png

prep:
	$(MKDIR_P) html
	$(MKDIR_P) xml

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(gr_docdir)
	cp -r html $(DESTDIR)$(gr_docdir)
	cp -r xml $(DESTDIR)$(gr_docdir)

uninstall-local:
	$(RM) -fr $(DESTDIR)$(gr_docdir)/html
	$(RM) -fr $(DESTDIR)$(gr_docdir)/xml

clean-local:
	$(RM) -fr latex html man xml
