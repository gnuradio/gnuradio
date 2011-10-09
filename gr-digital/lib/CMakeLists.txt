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
    ${GR_DIGITAL_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_digital_sources
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
    digital_fll_band_edge_cc.cc
    digital_lms_dd_equalizer_cc.cc
    digital_kurtotic_equalizer_cc.cc
    digital_mpsk_receiver_cc.cc
    digital_gmskmod_bc.cc
    digital_cpmmod_bc.cc
)

list(APPEND digital_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-digital SHARED ${gr_digital_sources})
target_link_libraries(gnuradio-digital ${digital_libs})
set_target_properties(gnuradio-digital PROPERTIES DEFINE_SYMBOL "gnuradio_digital_EXPORTS")
set_target_properties(gnuradio-digital PROPERTIES SOVERSION ${LIBVER})

install(TARGETS gnuradio-digital
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "digital_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "digital_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "digital_runtime" # .dll file
)
