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

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_VOCODER_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_vocoder_sources
    vocoder_alaw_decode_bs.cc
    vocoder_alaw_encode_sb.cc
    vocoder_codec2_decode_ps.cc
    vocoder_codec2_encode_sp.cc
    vocoder_cvsd_decode_bs.cc
    vocoder_cvsd_encode_sb.cc
    vocoder_g721_decode_bs.cc
    vocoder_g721_encode_sb.cc
    vocoder_g723_24_decode_bs.cc
    vocoder_g723_24_encode_sb.cc
    vocoder_g723_40_decode_bs.cc
    vocoder_g723_40_encode_sb.cc
    vocoder_gsm_fr_decode_ps.cc
    vocoder_gsm_fr_encode_sp.cc
    vocoder_ulaw_decode_bs.cc
    vocoder_ulaw_encode_sb.cc
)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
GR_INCLUDE_SUBDIRECTORY(codec2)
GR_INCLUDE_SUBDIRECTORY(g7xx)
GR_INCLUDE_SUBDIRECTORY(gsm)

list(APPEND vocoder_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-vocoder SHARED ${gr_vocoder_sources})
target_link_libraries(gnuradio-vocoder ${vocoder_libs})
GR_LIBRARY_FOO(gnuradio-vocoder RUNTIME_COMPONENT "vocoder_runtime" DEVEL_COMPONENT "vocoder_devel")
