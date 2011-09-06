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
    ${GR_TRELLIS_INCLUDE_DIRS}
    ${GR_DIGITAL_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

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

GEGEN_GEN_EXTRACT(generated_trellis_sources ".cc")
GEGEN_GEN_EXTRACT(generated_trellis_includes ".h")
GEGEN_GEN_EXTRACT(generated_trellis_swigs ".i")

#TODO simplify this list with a triple-threat for loop
SET(generated_trellis_deps
    generate_all.py
    generate_trellis.py
    trellis_encoder_XX.cc.t
    trellis_encoder_XX.h.t
    trellis_encoder_XX.i.t
    trellis_sccc_encoder_XX.cc.t
    trellis_sccc_encoder_XX.h.t
    trellis_sccc_encoder_XX.i.t
    trellis_pccc_encoder_XX.cc.t
    trellis_pccc_encoder_XX.h.t
    trellis_pccc_encoder_XX.i.t
    trellis_metrics_X.cc.t
    trellis_metrics_X.h.t
    trellis_metrics_X.i.t
    trellis_viterbi_combined_XX.cc.t
    trellis_viterbi_combined_XX.h.t
    trellis_viterbi_combined_XX.i.t
    trellis_sccc_decoder_combined_XX.cc.t
    trellis_sccc_decoder_combined_XX.h.t
    trellis_sccc_decoder_combined_XX.i.t
    trellis_sccc_decoder_X.cc.t
    trellis_sccc_decoder_X.h.t
    trellis_sccc_decoder_X.i.t
    trellis_pccc_decoder_X.cc.t
    trellis_pccc_decoder_X.h.t
    trellis_pccc_decoder_X.i.t
    trellis_pccc_decoder_combined_XX.cc.t
    trellis_pccc_decoder_combined_XX.h.t
    trellis_pccc_decoder_combined_XX.i.t
    trellis_viterbi_X.cc.t
    trellis_viterbi_X.h.t
    trellis_viterbi_X.i.t
)

ADD_CUSTOM_COMMAND(
    OUTPUT
        ${generated_trellis_sources}
        ${generated_trellis_includes}
        ${generated_trellis_swigs}
    DEPENDS ${generated_trellis_deps}
    COMMAND ${PYTHON_EXECUTABLE} -B -c
    \"import os, sys\;sys.path.append('${GR_CORE_PYTHONPATH}')\;sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')\;os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'\;from generate_all import generate_all\;generate_all()\"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "generating trellis files"
)

ADD_CUSTOM_TARGET(trellis_generated DEPENDS
    ${generated_trellis_sources}
    ${generated_trellis_includes}
    ${generated_trellis_swigs}
)

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_trellis_sources
    fsm.cc
    quicksort_index.cc
    base.cc
    interleaver.cc
    calc_metric.cc
    core_algorithms.cc
    trellis_permutation.cc
    trellis_siso_f.cc
    trellis_siso_combined_f.cc
    trellis_constellation_metrics_cf.cc
    ${generated_trellis_sources}
)

LIST(APPEND trellis_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

ADD_LIBRARY(gnuradio-trellis SHARED ${gr_trellis_sources})
TARGET_LINK_LIBRARIES(gnuradio-trellis ${trellis_libs})
SET_TARGET_PROPERTIES(gnuradio-trellis PROPERTIES DEFINE_SYMBOL "gnuradio_trellis_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-trellis PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-trellis
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "trellis_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "trellis_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "trellis_runtime" # .dll file
)

########################################################################
# Handle the generated sources + a few non-generated ones
########################################################################
INSTALL(FILES
    ${generated_trellis_includes}
    trellis_api.h
    fsm.h
    quicksort_index.h
    base.h
    interleaver.h
    calc_metric.h
    core_algorithms.h
    trellis_permutation.h
    siso_type.h
    trellis_siso_f.h
    trellis_siso_combined_f.h
    trellis_constellation_metrics_cf.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "trellis_devel"
)

IF(ENABLE_PYTHON)
    INSTALL(FILES
        ${generated_trellis_swigs}
        fsm.i
        interleaver.i
        trellis_permutation.i
        trellis_siso_f.i
        trellis_siso_combined_f.i
        trellis_constellation_metrics_cf.i
        ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.i
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
        COMPONENT "trellis_swig"
    )

########################################################################
# Setup swig generation
########################################################################
INCLUDE(GrPython)
INCLUDE(GrSwig)

SET(GR_SWIG_INCLUDE_DIRS
    ${GR_TRELLIS_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
    ${GR_DIGITAL_SWIG_INCLUDE_DIRS}
)

SET(GR_SWIG_LIBRARIES gnuradio-trellis)

SET(GR_SWIG_TARGET_DEPS trellis_generated)

GR_SWIG_MAKE(trellis trellis.i)

GR_SWIG_INSTALL(
    TARGETS trellis
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "trellis_python"
)

INSTALL(
    FILES trellis.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "trellis_swig"
)

ENDIF(ENABLE_PYTHON)
