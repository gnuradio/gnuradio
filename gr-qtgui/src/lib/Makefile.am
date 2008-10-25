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

include $(top_srcdir)/Makefile.common

#SUBDIRS=qtspectrum
#DIST_SUBDIRS=qtspectrum

# Install this stuff so that it ends up as the gnuradio.qtgui module
# This usually ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/qtgui

ourpythondir = $(grpythondir)/qtgui
ourlibdir    = $(grpyexecdir)/qtgui

AM_CPPFLAGS = $(STD_DEFINES_AND_INCLUDES) $(PYTHON_CPPFLAGS) \
              $(QT_INCLUDES) -I. $(WITH_INCLUDES)

SWIGPYTHONARGS = $(SWIGPYTHONFLAGS) $(STD_DEFINES_AND_INCLUDES) \
	$(WITH_SWIG_INCLUDES) $(WITH_INCLUDES)

ALL_IFILES = 				\
	$(LOCAL_IFILES)			\
	$(NON_LOCAL_IFILES)		

NON_LOCAL_IFILES =			\
	$(GNURADIO_I)

LOCAL_IFILES = 				\
	$(top_srcdir)/gr-qtgui/src/lib/qtgui.i

# These files are built by SWIG.  The first is the C++ glue.
# The second is the python wrapper that loads the shared library
# and knows how to call our extensions.

BUILT_SOURCES = 				\
	qtgui.cc				\
	qtgui.py				\
	spectrumdisplayform_moc.cc		\
	FrequencyDisplayPlot_moc.cc	 	\
	TimeDomainDisplayPlot_moc.cc		\
	WaterfallDisplayPlot_moc.cc		\
	Waterfall3DDisplayPlot_moc.cc		\
	spectrumdisplayform_ui.h		

# This gets qtgui.py installed in the right place
ourpython_PYTHON =			\
	qtgui.py

# Build the library for Python module to link against
ourlib_LTLIBRARIES = _qtgui.la

# Build the normal library for C++ apps to link against
lib_LTLIBRARIES = libqtgui.la

# These are the source files that go into the shared library
libqtgui_la_SOURCES = 	\
	spectrumdisplayform.cc			\
	FrequencyDisplayPlot.cc 		\
	TimeDomainDisplayPlot.cc		\
	WaterfallDisplayPlot.cc			\
	Waterfall3DDisplayPlot.cc		\
	waterfallGlobalData.cc			\
	SpectrumGUIClass.cc			\
	spectrumUpdateEvents.cc			\
	plot_waterfall.cc			\
	spectrumdisplayform.ui			\
	spectrumdisplayform_moc.cc		\
	FrequencyDisplayPlot_moc.cc	 	\
	TimeDomainDisplayPlot_moc.cc		\
	WaterfallDisplayPlot_moc.cc		\
	Waterfall3DDisplayPlot_moc.cc		\
	qtgui_sink_c.cc				\
	qtgui_sink_f.cc

_qtgui_la_SOURCES = 			\
	$(libqtgui_la_SOURCES)		\
	qtgui.cc

# These headers get installed in ${prefix}/include/gnuradio
grinclude_HEADERS =			\
	spectrumdisplayform.h		\
	FrequencyDisplayPlot.h		\
	TimeDomainDisplayPlot.h		\
	WaterfallDisplayPlot.h		\
	Waterfall3DDisplayPlot.h	\
	SpectrumGUIClass.h		\
	waterfallGlobalData.h		\
	highResTimeFunctions.h		\
	plot_waterfall.h		\
	spectrumUpdateEvents.h		\
	qtgui.h				\
	qtgui_sink_c.h			\
	qtgui_sink_f.h

%_moc.cc : %.h
	$(QT_MOC_EXEC) -DQT_SHARED -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB $< -o $@

%_ui.h : %.ui
	$(QT_UIC_EXEC) $< -o $@

# magic flags
_qtgui_la_LDFLAGS = $(NO_UNDEFINED) -module -avoid-version
libqtgui_la_LDFLAGS = $(NO_UNDEFINED) -version-info 0:0:0

# link the library against the c++ standard library
_qtgui_la_LIBADD = 		\
	$(PYTHON_LDFLAGS)	\
	$(GNURADIO_CORE_LA)	\
	-lstdc++		\
	$(QT_LIBS)


libqtgui_la_LIBADD = 		\
	$(GNURADIO_CORE_LA)	\
	-lstdc++		\
	$(QT_LIBS)

qtgui.cc qtgui.py: qtgui.i $(ALL_IFILES)
	$(SWIG) $(SWIGPYTHONARGS) -module qtgui -o qtgui.cc $(LOCAL_IFILES)

# These swig headers get installed in ${prefix}/include/gnuradio/swig
swiginclude_HEADERS = 			\
	$(LOCAL_IFILES)


MOSTLYCLEANFILES = $(BUILT_SOURCES) *.pyc

#EXTRA_DIST=qtspectrum/GNUmakefile

# Don't distribute output of swig
dist-hook:
	@for file in $(BUILT_SOURCES); do echo $(RM) $(distdir)/$$file; done
	@for file in $(BUILT_SOURCES); do $(RM) $(distdir)/$$file; done
