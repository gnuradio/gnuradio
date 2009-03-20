#
# Copyright 2004,2005,2008,2009 Free Software Foundation, Inc.
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

# Install this stuff so that it ends up as the gnuradio.wxgui module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/wxgui

ourpythondir = $(grpythondir)/wxgui/plotter
ourlibdir    = $(grpyexecdir)/wxgui/plotter

ourpython_PYTHON =			\
	__init__.py			\
	bar_plotter.py		\
	channel_plotter.py		\
	common.py			\
	gltext.py			\
	grid_plotter_base.py	\
	plotter_base.py			\
	waterfall_plotter.py

