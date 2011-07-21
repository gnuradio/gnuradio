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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_VOCODER_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_vocoder_sources
    vocoder_alaw_decode_bs.cc
    vocoder_alaw_encode_sb.cc
    vocoder_codec2_decode_ps.cc
    vocoder_codec2_encode_sp.cc
    vocoder_cvsd_encode_sb.cc
    vocoder_cvsd_decode_bs.cc
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

LIST(APPEND vocoder_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-vocoder SHARED ${gr_vocoder_sources})
TARGET_LINK_LIBRARIES(gnuradio-vocoder ${vocoder_libs})
SET_TARGET_PROPERTIES(gnuradio-vocoder PROPERTIES DEFINE_SYMBOL "gnuradio_vocoder_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-vocoder PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-vocoder
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "vocoder_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "vocoder_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "vocoder_runtime" # .dll file
)
