# Copyright 2012 Free Software Foundation, Inc.
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
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${GR_TRELLIS_INCLUDE_DIRS}
    ${GR_DIGITAL_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${LOG4CXX_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})
link_directories(${LOG4CXX_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

#######################################################################
# generate the python helper script which calls into the build utils
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_impl_dict2(name, sig, 'trellis')
        build_utils.expand_template(d, inp)
")

macro(expand_cc root)
  #make a list of all the generated files
  unset(expanded_files_cc)
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc ${CMAKE_CURRENT_BINARY_DIR}/${name}.cc)
    list(APPEND expanded_files_h  ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  
  #create a command to generate the source files
  add_custom_command(
    OUTPUT ${expanded_files_cc}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.cc.t ${ARGN}
  )

  #create a command to generate the header file
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
  )
  
  #make source files depends on headers to force generation
  set_source_files_properties(${expanded_files_cc}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )
  
  #install rules for the generated cc files
  list(APPEND generated_sources ${expanded_files_cc})  
  list(APPEND generated_headers ${expanded_files_h})  
endmacro(expand_cc)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_cc(encoder_XX_impl                bb bs bi ss si ii)
expand_cc(sccc_encoder_XX_impl           bb bs bi ss si ii)
expand_cc(pccc_encoder_XX_impl           bb bs bi ss si ii)
expand_cc(metrics_X_impl                 s i f c)
expand_cc(viterbi_X_impl                 b s i)
expand_cc(viterbi_combined_XX_impl       sb ss si ib is ii fb fs fi cb cs ci)
expand_cc(sccc_decoder_X_impl            b s i)
expand_cc(sccc_decoder_combined_XX_impl  fb fs fi cb cs ci)
expand_cc(pccc_decoder_X_impl            b s i)
expand_cc(pccc_decoder_combined_XX_impl  fb fs fi cb cs ci)

########################################################################
# Setup library
########################################################################
list(APPEND trellis_sources
    ${generated_sources}
    base.cc
    calc_metric.cc
    core_algorithms.cc
    fsm.cc
    interleaver.cc
    quicksort_index.cc
    constellation_metrics_cf_impl.cc
    permutation_impl.cc
    siso_f_impl.cc
    siso_combined_f_impl.cc
)

list(APPEND trellis_libs
    gnuradio-runtime
    gnuradio-digital
    ${Boost_LIBRARIES}
)

#Add Windows DLL resource file if using MSVC
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-trellis.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc
    @ONLY)

    list(APPEND trellis_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-trellis.rc
    )
ENDIF(MSVC)

add_library(gnuradio-trellis SHARED ${trellis_sources})
target_link_libraries(gnuradio-trellis ${trellis_libs})
GR_LIBRARY_FOO(gnuradio-trellis RUNTIME_COMPONENT "trellis_runtime" DEVEL_COMPONENT "trellis_devel")
add_dependencies(gnuradio-trellis
  trellis_generated_includes trellis_generated_swigs
  gnuradio-runtime gnuradio-digital)
