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
# Generate the makefile.gen, then extract its sources:
# This is a round-about way to extract the sources,
# but it requires minimum changed to the python utils.
#
# The recommended way to do this:
#   - Make a generation macro that registers the sources command.
#   - List the generation macro with each templated source file.
#   - Create a python script (very generic) to perform generation.
#   - This way the targets would depend only on their sources.
########################################################################
EXECUTE_PROCESS(
    COMMAND ${PYTHON_EXECUTABLE} -c "
import os, sys
sys.path.append('${GR_CORE_PYTHONPATH}')
sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.environ['gendir'] = '${CMAKE_CURRENT_BINARY_DIR}'
os.environ['do_makefile'] = '1'
os.environ['do_sources'] = '0'
from generate_all import generate_all
generate_all()
    " WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

MACRO(GEGEN_GEN_EXTRACT outvar ext)
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import os; print ';'.join(
        map(lambda x: os.path.join('${CMAKE_CURRENT_BINARY_DIR}', x.replace('\\\\', '').strip()),
        filter(lambda f: '${ext}' in f, open('${CMAKE_CURRENT_BINARY_DIR}/Makefile.gen').readlines()
    )))" OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE ${outvar})
    FILE(TO_CMAKE_PATH "${${outvar}}" ${outvar})
ENDMACRO(GEGEN_GEN_EXTRACT)

GEGEN_GEN_EXTRACT(generated_gengen_sources ".cc")
GEGEN_GEN_EXTRACT(generated_gengen_includes ".h")
GEGEN_GEN_EXTRACT(generated_gengen_swigs ".i")

#TODO simplify this list with a triple-threat for loop
SET(generated_gengen_deps
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_all.py
    ${CMAKE_CURRENT_SOURCE_DIR}/generate_common.py
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_vXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_vXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_add_const_vXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_argmax_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_argmax_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_argmax_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_chunks_to_symbols_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_chunks_to_symbols_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_chunks_to_symbols_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_divide_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_divide_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_divide_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_integrate_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_integrate_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_integrate_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_max_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_max_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_max_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_vXX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_vXX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_multiply_const_vXX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_mute_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_mute_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_mute_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_noise_source_X.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_noise_source_X.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_noise_source_X.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_packed_to_unpacked_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_packed_to_unpacked_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_packed_to_unpacked_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_peak_detector_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_peak_detector_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_peak_detector_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sample_and_hold_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sample_and_hold_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sample_and_hold_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sig_source_X.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sig_source_X.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sig_source_X.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sub_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sub_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sub_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_unpacked_to_packed_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_unpacked_to_packed_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_unpacked_to_packed_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_source_X.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_source_X.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_source_X.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_sink_X.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_sink_X.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vector_sink_X.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_xor_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_xor_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_xor_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_const_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_const_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_and_const_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_or_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_or_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_or_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_not_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_not_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_not_XX.i.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_moving_average_XX.cc.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_moving_average_XX.h.t
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_moving_average_XX.i.t
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        ${generated_gengen_sources}
        ${generated_gengen_includes}
        ${generated_gengen_swigs}
    DEPENDS ${generated_gengen_deps}
    COMMAND ${PYTHON_EXECUTABLE} -B -c
    \"import os, sys\;sys.path.append('${GR_CORE_PYTHONPATH}')\;sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')\;os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'\;from generate_all import generate_all\;generate_all()\"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "generating gengen files"
)

ADD_CUSTOM_TARGET(gengen_generated DEPENDS
    ${generated_gengen_sources}
    ${generated_gengen_includes}
    ${generated_gengen_swigs}
)

########################################################################
# Handle the generated sources + a few non-generated ones
########################################################################
LIST(APPEND gnuradio_core_sources
    ${generated_gengen_sources}
)

INSTALL(FILES
    ${generated_gengen_includes}
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_endianness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_noise_type.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sig_source_waveform.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

IF(ENABLE_PYTHON)
    INSTALL(FILES
        ${generated_gengen_swigs}
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_endianness.i
        ${CMAKE_CURRENT_SOURCE_DIR}/gengen.i
        ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
        COMPONENT "core_swig"
    )
ENDIF(ENABLE_PYTHON)
