#
# Copyright 2004,2005 Free Software Foundation, Inc.
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

docdir=$(prefix)/share/doc/@PACKAGE@-@VERSION@

HTML_FILES =

if HAS_XMLTO
HTML_FILES += \
    gr-trellis.html
endif

all: $(HTML_FILES)

EXTRA_DIST = \
    gr-trellis.xml \
    make_numbered_listing.py \
    test_tcm.py \
    test_tcm.py.xml \
    test_viterbi_equalization1.py \
    test_viterbi_equalization1.py.xml

BUILT_XML_FILES = 			

# ----------------------------------------------------------------

gr-trellis.html : gr-trellis.xml
	xmlto html-nochunks $(top_srcdir)/gr-trellis/doc/gr-trellis.xml

# XXX `%'-style pattern rules are a GNU make extension
%.xml : % make_numbered_listing.py
	./make_numbered_listing.py $<

install-data-local:
	mkdir -p $(DESTDIR)$(docdir)/html
	@for i in $(HTML_FILES); do \
		echo "$(INSTALL_DATA) $$i $(DESTDIR)$(docdir)/html"; \
		$(INSTALL_DATA) $$i $(DESTDIR)$(docdir)/html; \
	done

uninstall-local:
	@for i in $(HTML_FILES); do \
		echo "$(RM) $(DESTDIR)$(docdir)/$$i;"; \
		$(RM) $(DESTDIR)$(docdir)/$$i; \
	done
	$(RM) -fr $(DESTDIR)$(docdir)/html

clean-local:
	$(RM) -fr $(HTML_FILES) *~
