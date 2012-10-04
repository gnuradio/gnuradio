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
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)

########################################################################
# generate helper scripts to expand templated files
########################################################################
include(GrPython)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_CORE_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict(name, sig, 'blocks')
        build_utils.expand_template(d, inp, '_impl')
")

macro(expand_cc_h_impl root)
  #make a list of all the generated files
  unset(expanded_files_cc_impl)
  unset(expanded_files_h_impl)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.cc)
    list(APPEND expanded_files_h_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.h)
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/../include/${name}.h)
  endforeach(sig)
  
  #create a command to generate the _impl.cc files
  add_custom_command(
    OUTPUT ${expanded_files_cc_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.cc.t ${ARGN}
  )
  
  #create a command to generate the _impl.h files
  add_custom_command(
    OUTPUT ${expanded_files_h_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.h.t ${ARGN}
  )
  
  #make _impl.cc source files depend on headers to force generation
  set_source_files_properties(${expanded_files_cc_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h_impl}"
  )
  
  #make _impl.h source files depend on headers to force generation
  set_source_files_properties(${expanded_files_h_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )

  #install rules for the generated cc files
  list(APPEND generated_sources ${expanded_files_cc_impl})  
endmacro(expand_cc_h_impl)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_cc_h_impl(add_XX             ss ii cc)
expand_cc_h_impl(add_const_XX       ss ii ff cc)
expand_cc_h_impl(add_const_vXX      ss ii ff cc)
expand_cc_h_impl(and_XX             bb ss ii)
expand_cc_h_impl(and_const_XX       bb ss ii)
expand_cc_h_impl(divide_XX          ss ii ff cc)
expand_cc_h_impl(integrate_XX       ss ii ff cc)
expand_cc_h_impl(multiply_XX        ss ii)
expand_cc_h_impl(multiply_const_XX  ss ii)
expand_cc_h_impl(multiply_const_vXX ss ii ff cc)
expand_cc_h_impl(not_XX             bb ss ii)
expand_cc_h_impl(or_XX              bb ss ii)
expand_cc_h_impl(sub_XX             ss ii ff cc)
expand_cc_h_impl(xor_XX             bb ss ii)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

include_directories(${BLOCKS_INCLUDE_DIRS})
link_directories(${BLOCKS_LIBRARY_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_blocks_sources
    ${generated_sources}
    add_ff_impl.cc
    char_to_float_impl.cc
    char_to_short_impl.cc
    complex_to_interleaved_short_impl.cc
    complex_to_float_impl.cc
    complex_to_real_impl.cc
    complex_to_imag_impl.cc
    complex_to_mag_impl.cc
    complex_to_mag_squared_impl.cc
    complex_to_arg_impl.cc
    conjugate_cc_impl.cc
    deinterleave_impl.cc
    file_source_impl.cc
    float_to_char_impl.cc
    float_to_complex_impl.cc
    float_array_to_int.cc
    float_to_int_impl.cc
    float_to_short_impl.cc
    float_array_to_uchar.cc
    float_to_uchar_impl.cc
    int_to_float_impl.cc
    interleave_impl.cc
    interleaved_short_array_to_complex.cc
    interleaved_short_to_complex_impl.cc
    keep_m_in_n_impl.cc
    keep_one_in_n_impl.cc
    multiply_cc_impl.cc
    multiply_ff_impl.cc
    multiply_conjugate_cc_impl.cc
    multiply_const_cc_impl.cc
    multiply_const_ff_impl.cc
    nlog10_ff_impl.cc
    repeat_impl.cc
    short_to_char_impl.cc
    short_to_float_impl.cc
    stream_mux_impl.cc
    stream_to_streams_impl.cc
    stream_to_vector_impl.cc
    streams_to_stream_impl.cc
    streams_to_vector_impl.cc
    uchar_array_to_float.cc
    uchar_to_float_impl.cc
    vector_to_stream_impl.cc
    vector_to_streams_impl.cc
)

list(APPEND blocks_libs
    gnuradio-core
    volk
    ${Boost_LIBRARIES}
    ${BLOCKS_LIBRARIES}
)

add_library(gnuradio-blocks SHARED ${gr_blocks_sources})
add_dependencies(gnuradio-blocks blocks_generated_includes)

target_link_libraries(gnuradio-blocks ${blocks_libs})
GR_LIBRARY_FOO(gnuradio-blocks RUNTIME_COMPONENT "blocks_runtime" DEVEL_COMPONENT "blocks_devel")
