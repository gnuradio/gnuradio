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

TESTS =
EXTRA_DIST += $(nobase_guile_DATA)

AM_CPPFLAGS = \
	-I$(abs_top_srcdir)/gr-vocoder/include \
	$(STD_DEFINES_AND_INCLUDES) \
	$(PYTHON_CPPFLAGS) \
	$(WITH_INCLUDES)

if GUILE
nobase_guile_DATA = \
	gnuradio/vocoder_swig.scm
endif

noinst_GUILE  = vocoder.test


##############################
# SWIG interface and library

TOP_SWIG_DOC_IFILES = \
	vocoder_swig_doc.i

TOP_SWIG_IFILES = \
	$(TOP_SWIG_DOC_IFILES) 	\
	vocoder_swig.i

BUILT_SOURCES += \
	$(TOP_SWIG_DOC_IFILES)

EXTRA_DIST += \
	$(TOP_SWIG_DOC_IFILES)

$(TOP_SWIG_DOC_IFILES):
	`echo "" > $@`

# Install so that they end up available as:
#   import gnuradio.vocoder
# This ends up at:
#   ${prefix}/lib/python${python_version}/site-packages/gnuradio/vocoder
vocoder_swig_pythondir_category = \
	gnuradio/vocoder

# additional libraries for linking with the SWIG-generated library
vocoder_swig_la_swig_libadd = \
	$(abs_top_builddir)/gr-vocoder/lib/libgnuradio-vocoder.la

# additional SWIG files to be installed
vocoder_swig_swiginclude_headers = 	\
	vocoder_alaw_decode_bs.i 	\
	vocoder_alaw_encode_sb.i 	\
	vocoder_codec2_decode_ps.i 	\
	vocoder_codec2_encode_sp.i 	\
	vocoder_cvsd_decode_bs.i 	\
	vocoder_cvsd_encode_sb.i 	\
	vocoder_g721_decode_bs.i 	\
	vocoder_g721_encode_sb.i 	\
	vocoder_g723_24_decode_bs.i 	\
	vocoder_g723_24_encode_sb.i 	\
	vocoder_g723_40_decode_bs.i 	\
	vocoder_g723_40_encode_sb.i 	\
	vocoder_gsm_fr_encode_sp.i 	\
	vocoder_gsm_fr_decode_ps.i 	\
	vocoder_ulaw_decode_bs.i 	\
	vocoder_ulaw_encode_sb.i	\
	$(TOP_SWIG_DOC_IFILE)

vocoder_swig_swig_args = \
	-I$(abs_top_builddir)/gr-vocoder/lib

if GUILE
TESTS += run_guile_tests
endif
