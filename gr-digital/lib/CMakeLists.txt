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
    ${GR_DIGITAL_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_digital_sources
    digital_binary_slicer_fb.cc
    digital_clock_recovery_mm_cc.cc
    digital_clock_recovery_mm_ff.cc
    digital_constellation.cc
    digital_constellation_receiver_cb.cc
    digital_constellation_decoder_cb.cc
    digital_correlate_access_code_bb.cc
    digital_costas_loop_cc.cc
    digital_cma_equalizer_cc.cc
    digital_crc32.cc
    digital_lms_dd_equalizer_cc.cc
    digital_kurtotic_equalizer_cc.cc
    digital_mpsk_receiver_cc.cc
)

LIST(APPEND digital_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-digital SHARED ${gr_digital_sources})
TARGET_LINK_LIBRARIES(gnuradio-digital ${digital_libs})
SET_TARGET_PROPERTIES(gnuradio-digital PROPERTIES DEFINE_SYMBOL "gnuradio_digital_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-digital PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-digital
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "digital_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "digital_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "digital_runtime" # .dll file
)

########################################################################
# Install header files
########################################################################
INSTALL(FILES
    digital_binary_slicer_fb.h
    digital_clock_recovery_mm_cc.h
    digital_clock_recovery_mm_ff.h
    digital_constellation.h
    digital_constellation_receiver_cb.h
    digital_constellation_decoder_cb.h
    digital_correlate_access_code_bb.h
    digital_costas_loop_cc.h
    digital_cma_equalizer_cc.h
    digital_crc32.h
    digital_lms_dd_equalizer_cc.h
    digital_kurtotic_equalizer_cc.h
    digital_metric_type.h
    digital_mpsk_receiver_cc.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "digital_devel"
)
