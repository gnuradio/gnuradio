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
    ${GR_TRELLIS_INCLUDE_DIRS}
    ${GR_DIGITAL_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# generate the python helper script which calls into the build utils
########################################################################
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_CORE_PYTHONPATH}')
sys.path.append('${CMAKE_CURRENT_SOURCE_DIR}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils, generate_trellis
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = generate_trellis.standard_dict(name, sig)
        build_utils.expand_template(d, inp)

")

########################################################################
# generation helper macro to generate various files from template
########################################################################
macro(expand_h_cc_i root)

    foreach(ext h cc i)
        #make a list of all the generated files
        unset(expanded_files_${ext})
        foreach(sig ${ARGN})
            string(REGEX REPLACE "X+" ${sig} name ${root})
            list(APPEND expanded_files_${ext} ${CMAKE_CURRENT_BINARY_DIR}/${name}.${ext})
        endforeach(sig)

        #create a command to generate the files
        add_custom_command(
            OUTPUT ${expanded_files_${ext}}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.${ext}.t
            COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
                ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
                ${root} ${root}.${ext}.t ${ARGN}
        )
    endforeach(ext)

    #make source files depends on headers to force generation
    set_source_files_properties(${expanded_files_cc}
        PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
    )

    #install rules for the generated cc, h, and i files
    list(APPEND generated_trellis_sources ${expanded_files_cc})
    list(APPEND generated_trellis_includes ${expanded_files_h})
    list(APPEND generated_trellis_swigs ${expanded_files_i})

endmacro(expand_h_cc_i)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_h_cc_i(trellis_encoder_XX                 bb bs bi ss si ii)
expand_h_cc_i(trellis_sccc_encoder_XX            bb bs bi ss si ii)
expand_h_cc_i(trellis_pccc_encoder_XX            bb bs bi ss si ii)
expand_h_cc_i(trellis_metrics_X                  s i f c)
expand_h_cc_i(trellis_viterbi_X                  b s i)
expand_h_cc_i(trellis_viterbi_combined_XX        sb ss si ib is ii fb fs fi cb cs ci)
expand_h_cc_i(trellis_sccc_decoder_X             b s i)
expand_h_cc_i(trellis_sccc_decoder_combined_XX   fb fs fi cb cs ci)
expand_h_cc_i(trellis_pccc_decoder_X             b s i)
expand_h_cc_i(trellis_pccc_decoder_combined_XX   fb fs fi cb cs ci)

add_custom_target(trellis_generated DEPENDS
    ${generated_trellis_includes}
    ${generated_trellis_swigs}
)

########################################################################
# Create the master trellis swig include files
########################################################################
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.py "
import os, sys
if __name__ == '__main__':
    incs = sys.argv[2:]
    h_incs = '\\n'.join(['#include<%s.h>'%(os.path.splitext(os.path.basename(inc))[0]) for inc in incs])
    i_incs = '\\n'.join(['%%include<%s>'%(os.path.basename(inc)) for inc in incs])
    open(sys.argv[1], 'w').write('''
//
// This file is machine generated.  All edits will be overwritten
//

%%{
%s
%%}

%s

'''%(h_incs, i_incs))
")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.i
    DEPENDS ${generated_trellis_swigs}
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
        ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.py
        ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.i
        ${generated_trellis_swigs}
)

########################################################################
# Setup library
########################################################################
list(APPEND gr_trellis_sources
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

list(APPEND trellis_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-trellis SHARED ${gr_trellis_sources})
target_link_libraries(gnuradio-trellis ${trellis_libs})
set_target_properties(gnuradio-trellis PROPERTIES DEFINE_SYMBOL "gnuradio_trellis_EXPORTS")
set_target_properties(gnuradio-trellis PROPERTIES SOVERSION ${LIBVER})

install(TARGETS gnuradio-trellis
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "trellis_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "trellis_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "trellis_runtime" # .dll file
)

########################################################################
# Handle the generated sources + a few non-generated ones
########################################################################
install(FILES
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

if(ENABLE_PYTHON)
    install(FILES
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
include(GrPython)
include(GrSwig)

set(GR_SWIG_SOURCE_DEPS ${CMAKE_CURRENT_BINARY_DIR}/trellis_generated.i)
set(GR_SWIG_TARGET_DEPS trellis_generated trellis_generated_index)
set(GR_SWIG_INCLUDE_DIRS
    ${GR_TRELLIS_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
    ${GR_DIGITAL_SWIG_INCLUDE_DIRS}
)
set(GR_SWIG_LIBRARIES gnuradio-trellis)
GR_SWIG_MAKE(trellis trellis.i)

GR_SWIG_INSTALL(
    TARGETS trellis
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "trellis_python"
)

install(
    FILES trellis.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "trellis_swig"
)

endif(ENABLE_PYTHON)
