# Copyright 2010-2011 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_histo_sink_f.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_guts.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_sink_f.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_sink_x.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bitbang.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bbio.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bbio_pp.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_4702.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_4937.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_xxxx.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/ppio_ppdev.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_wavfile.cc
)

########################################################################
# Install runtime headers
########################################################################
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_histo_sink_f.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_guts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_sink_f.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_sink_x.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_trigger_mode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c.h
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bitbang.h
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bbio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/i2c_bbio_pp.h
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_4702.h
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_4937.h
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_eval_board_defs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/microtune_xxxx.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ppio_ppdev.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_wavfile.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/io.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_histo_sink.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_oscope_sink.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
endif(ENABLE_PYTHON)

########################################################################
# Handle triple-threat files that have cc, h, and i
########################################################################
set(gr_core_io_triple_threats
    gr_file_sink
    gr_file_sink_base
    gr_file_source
    gr_file_descriptor_sink
    gr_file_descriptor_source
    gr_message_sink
    gr_message_source
    microtune_xxxx_eval_board
    microtune_4702_eval_board
    microtune_4937_eval_board
    ppio
    sdr_1000
    gr_udp_sink
    gr_udp_source
    gr_wavfile_source
    gr_wavfile_sink
    gr_tagged_file_sink
)

foreach(file_tt ${gr_core_io_triple_threats})
    list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.cc)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio COMPONENT "core_devel")
    if(ENABLE_PYTHON)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.i DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig COMPONENT "core_swig")
    endif(ENABLE_PYTHON)
endforeach(file_tt ${gr_core_io_triple_threats})
