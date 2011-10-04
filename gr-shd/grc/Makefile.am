#
# Copyright 2011 Free Software Foundation, Inc.
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

generated_shd_smini_blocks = \
	shd_smini_source.xml \
	shd_smini_sink.xml

BUILT_SOURCES += $(generated_shd_smini_blocks)

dist_grcblocks_DATA = \
	shd_block_tree.xml \
	$(BUILT_SOURCES)

########################################################################
# Rules for generating the source and sink xml wrappers
########################################################################
EXTRA_DIST += $(srcdir)/gen_shd_smini_blocks.py

$(generated_shd_smini_blocks): $(srcdir)/gen_shd_smini_blocks.py
	@echo "generating $@..."
	$(PYTHON) $< $@
