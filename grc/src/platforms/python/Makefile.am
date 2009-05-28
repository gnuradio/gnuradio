#
# Copyright 2008,2009 Free Software Foundation, Inc.
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

include $(top_srcdir)/grc/Makefile.inc

SUBDIRS = utils

ourpythondir = $(grc_src_prefix)/platforms/python

ourpython_PYTHON = \
	Block.py \
	Connection.py \
	Constants.py \
	FlowGraph.py \
	Generator.py \
	Param.py \
	Platform.py \
	Port.py \
	__init__.py

Constants.py: $(srcdir)/Constants.py.in Makefile
	sed \
		-e 's|@PYTHONW[@]|$(PYTHONW)|g' \
		-e 's|@reldatadir[@]|$(grc_python_data_reldir)|g' \
		-e 's|@relblocksdir[@]|$(grc_blocks_data_reldir)|g' \
		-e 's|@docdir[@]|$(gr_docdir)|g' \
	$< > $@

EXTRA_DIST = $(srcdir)/Constants.py.in
BUILT_SOURCES = Constants.py
