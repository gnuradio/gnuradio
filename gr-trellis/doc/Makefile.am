#
# Copyright 2004,2005,2009 Free Software Foundation, Inc.
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

HTML_FILES =

if HAS_XMLTO
HTML_FILES += \
    gr-trellis.html
endif

all: $(HTML_FILES)

EXTRA_DIST += \
    gr-trellis.xml \
    make_numbered_listing.py \
    test_tcm.py \
    test_tcm.py.xml \
    test_viterbi_equalization1.py \
    test_viterbi_equalization1.py.xml

BUILT_XML_FILES = 			

htmldocdir = $(gr_docdir)/html
htmldoc_DATA = $(HTML_FILES)

# ----------------------------------------------------------------

gr-trellis.html : gr-trellis.xml
	xmlto html-nochunks $(top_srcdir)/gr-trellis/doc/gr-trellis.xml

test_tcm.py.xml : test_tcm.py make_numbered_listing.py
	$(srcdir)/make_numbered_listing.py $<

test_viterbi_equalization1.py.xml : test_viterbi_equalization1.py make_numbered_listing.py
	$(srcdir)/make_numbered_listing.py $<

clean-local:
	$(RM) -fr $(HTML_FILES) *~
