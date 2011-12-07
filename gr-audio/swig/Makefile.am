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
include $(top_srcdir)/Makefile.swig

AUDIO_CPPFLAGS = -I$(abs_top_srcdir)/gr-audio/include

AM_CPPFLAGS = \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(AUDIO_CPPFLAGS) \
	$(WITH_INCLUDES)

# ----------------------------------------------------------------
# The SWIG library

TOP_SWIG_DOC_IFILES = \
	audio_swig_doc.i

TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	audio_swig.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.audio
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/audio
audio_swig_pythondir_category = \
	gnuradio/audio

# additional libraries for linking with the SWIG-generated library
audio_swig_la_swig_libadd = \
	$(top_builddir)/gr-audio/lib/libgnuradio-audio.la

# additional Python files to be installed along with the SWIG-generated one
audio_swig_python = \
	__init__.py

# additional SWIG files to be installed
audio_swig_swiginclude_headers = \
	$(TOP_SWIG_DOC_IFILES)

audio_swig_swig_args = $(AUDIO_CPPFLAGS)
