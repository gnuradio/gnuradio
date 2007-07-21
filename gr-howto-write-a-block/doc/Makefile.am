#
# Copyright 2004,2005,2007 Free Software Foundation, Inc.
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

TARGETS = howto-write-a-block.html


# To avoid build problems for folks who don't have xmlto installed, we
# don't build the docs by default.

# html: $(TARGETS)
all: $(TARGETS)


EXTRA_DIST = 				\
	howto-write-a-block.xml		\
	howto_1.i			\
	make_numbered_listing.py	\
	qa_howto_1.py			\
	src_lib_Makefile_1.am		\
	src_lib_Makefile_2.am		


BUILT_XML_FILES = 			\
	gr_block.h.xml			\
	howto_1.i.xml			\
	howto_square_ff.cc.xml		\
	howto_square_ff.h.xml		\
	howto_square2_ff.cc.xml		\
	howto_square2_ff.h.xml		\
	qa_howto_1.py.xml		\
	src_lib_Makefile_1.am.xml	\
	src_lib_Makefile_2.am.xml	


howto-write-a-block.html : howto-write-a-block.xml $(BUILT_XML_FILES)


gr_block.h.xml: $(GNURADIO_CORE_INCLUDEDIR)/gr_block.h make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $(GNURADIO_CORE_INCLUDEDIR)/gr_block.h

howto_square_ff.cc.xml: $(top_srcdir)/src/lib/howto_square_ff.cc make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $(top_srcdir)/src/lib/howto_square_ff.cc 

howto_square_ff.h.xml: $(top_srcdir)/src/lib/howto_square_ff.h make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $(top_srcdir)/src/lib/howto_square_ff.h 

howto_square2_ff.cc.xml: $(top_srcdir)/src/lib/howto_square2_ff.cc make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $(top_srcdir)/src/lib/howto_square2_ff.cc 

howto_square2_ff.h.xml: $(top_srcdir)/src/lib/howto_square2_ff.h make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $(top_srcdir)/src/lib/howto_square2_ff.h 


# ----------------------------------------------------------------

clean:
	-${RM} -f $(TARGETS) $(BUILT_XML_FILES)

%.html : %.xml
	xmlto html-nochunks $<

%.xml : % make_numbered_listing.py
	$(PYTHON) ./make_numbered_listing.py $<
