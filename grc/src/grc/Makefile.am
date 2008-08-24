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

include $(top_srcdir)/grc/Makefile.inc

SUBDIRS = elements gui

ourpythondir = $(pythondir)/grc

ourpython_PYTHON = \
	__init__.py \
	Constants.py \
	converter.py \
	ActionHandler.py \
	Actions.py \
	Messages.py \
	ParseXML.py \
	Preferences.py \
	StateCache.py \
	Utils.py

BUILT_SOURCES = Constants.py

Constants.py: Makefile $(srcdir)/Constants.py.in
	sed \
		-e 's|@VERSION[@]|@VERSION@|g' \
		-e 's|@datadir[@]|$(grc_data_dir)|g' \
	$(srcdir)/Constants.py.in > $@

EXTRA_DIST = Constants.py.in

MOSTLYCLEANFILES = $(BUILT_SOURCES)
