# Copyright 2010-2012 Free Software Foundation, Inc.
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
# Handle the generated constants
########################################################################
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
    "import time;print time.strftime('%a, %d %b %Y %H:%M:%S', time.gmtime())"
    OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Loading build date ${BUILD_DATE} into gr_constants...")

message(STATUS "Loading version ${VERSION} into gr_constants...")

#double escape for windows backslash path separators
string(REPLACE "\\" "\\\\" prefix ${prefix})
string(REPLACE "\\" "\\\\" SYSCONFDIR ${SYSCONFDIR})
string(REPLACE "\\" "\\\\" GR_PREFSDIR ${GR_PREFSDIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc
@ONLY)

list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_BINARY_DIR}/gr_constants.cc)

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_circular_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_fast_atan2f.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_misc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_random.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_reverse.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_debugger_hook.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc16.c
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_general.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_circular_file.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_math.cc
)

########################################################################
# Install runtime headers
########################################################################
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_core_api.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_circular_file.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_endianness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_expj.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_math.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_misc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_random.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_reverse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_test_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_debugger_hook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_lfsr_15_1_0.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gri_lfsr_32k.h
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc16.h
    ${CMAKE_CURRENT_SOURCE_DIR}/random.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/general.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_constants.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
endif(ENABLE_PYTHON)

########################################################################
# Handle triple-threat files that have cc, h, and i
########################################################################
set(gr_core_general_triple_threats
    complex_vec_test
    gr_align_on_samplenumbers_ss
    gr_block_gateway
    gr_check_counting_s
    gr_check_lfsr_32k_s
    gr_fake_channel_coder_pp
    gr_feval
    gr_iqcomp_cc
    gr_lfsr_32k_source_s
    gr_pa_2x2_phase_combiner
    gr_prefs
    gr_test
)

foreach(file_tt ${gr_core_general_triple_threats})
    list(APPEND gnuradio_core_sources ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.cc)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio COMPONENT "core_devel")
    if(ENABLE_PYTHON)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file_tt}.i DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig COMPONENT "core_swig")
    endif(ENABLE_PYTHON)
endforeach(file_tt ${gr_core_general_triple_threats})
