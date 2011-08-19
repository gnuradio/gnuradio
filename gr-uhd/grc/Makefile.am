#
# Copyright 2010-2011 Free Software Foundation, Inc.
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

grcblocksdir = $(grc_blocksdir)

generated_uhd_usrp_blocks = \
	uhd_usrp_source.xml \
	uhd_usrp_sink.xml

BUILT_SOURCES += $(generated_uhd_usrp_blocks)

dist_grcblocks_DATA = \
	uhd_block_tree.xml \
	uhd_amsg_source.xml \
	$(BUILT_SOURCES)

########################################################################
# Rules for generating the source and sink xml wrappers
########################################################################
EXTRA_DIST += $(srcdir)/gen_uhd_usrp_blocks.py

$(generated_uhd_usrp_blocks): $(srcdir)/gen_uhd_usrp_blocks.py
	@echo "generating $@..."
	$(PYTHON) $< $@
