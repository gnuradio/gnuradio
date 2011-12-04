#
# Copyright 2007,2009,2011 Free Software Foundation, Inc.
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

EXTRA_DIST = \
	example/aadvark.cc 		\
	example/aadvark.h		\
	example/Doxyfile		\
	example/xml/aadvark_8cc.xml	\
	example/xml/aadvark_8h.xml	\
	example/xml/classAadvark.xml	\
	example/xml/combine.xslt	\
	example/xml/compound.xsd	\
	example/xml/index.xml		\
	example/xml/index.xsd

if PYTHON
utilspythondir = $(grpythondir)/doxyxml

TESTS = 				\
	run_tests

nobase_utilspython_PYTHON = \
	__init__.py \
	base.py \
	doxyindex.py \
	text.py \
	generated/__init__.py \
	generated/index.py \
	generated/indexsuper.py \
	generated/compound.py \
	generated/compoundsuper.py
endif