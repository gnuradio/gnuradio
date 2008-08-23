#
# Copyright 2008 Free Software Foundation, Inc.
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

include $(top_srcdir)/grc/Makefile.common

SUBDIRS = blks2 usrp utils wxgui

ourpythondir = $(pythondir)/grc_gnuradio

ourpython_PYTHON = \
	__init__.py \
	Constants.py \
	Block.py \
	Connection.py \
	FlowGraph.py \
	Generator.py \
	Platform.py \
	Param.py \
	Port.py

docdir = $(prefix)/share/doc/@PACKAGE@-@VERSION@

BUILT_SOURCES = Constants.py

Constants.py: Makefile $(srcdir)/Constants.py.in
	sed \
		-e 's|@PYTHONW[@]|$(PYTHONW)|g' \
		-e 's|@datadir[@]|$(grc_gnuradio_data_dir)|g' \
		-e 's|@blocksdir[@]|$(grc_gnuradio_blocks_dir)|g' \
		-e 's|@docdir[@]|$(docdir)|g' \
	$(srcdir)/Constants.py.in > $@

EXTRA_DIST = Constants.py.in

MOSTLYCLEANFILES = $(BUILT_SOURCES)
