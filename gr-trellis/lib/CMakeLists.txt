# Copyright 2012,2014 Free Software Foundation, Inc.
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
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

#######################################################################
# Invoke macro to generate various sources and headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_CC_H(trellis encoder_XX_impl                bb bs bi ss si ii)
GR_EXPAND_X_CC_H(trellis sccc_encoder_XX_impl           bb bs bi ss si ii)
GR_EXPAND_X_CC_H(trellis pccc_encoder_XX_impl           bb bs bi ss si ii)
GR_EXPAND_X_CC_H(trellis metrics_X_impl                 s i f c)
GR_EXPAND_X_CC_H(trellis viterbi_X_impl                 b s i)
GR_EXPAND_X_CC_H(trellis viterbi_combined_XX_impl       sb ss si ib is ii fb fs fi cb cs ci)
GR_EXPAND_X_CC_H(trellis sccc_decoder_X_impl            b s i)
GR_EXPAND_X_CC_H(trellis sccc_decoder_combined_XX_impl  fb fs fi cb cs ci)
GR_EXPAND_X_CC_H(trellis pccc_decoder_X_impl            b s i)
GR_EXPAND_X_CC_H(trellis pccc_decoder_combined_XX_impl  fb fs fi cb cs ci)

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

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-trellis APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-trellis_static STATIC ${trellis_sources})

  add_dependencies(gnuradio-trellis_static
    trellis_generated_includes trellis_generated_swigs
    gnuradio-runtime_static gnuradio-digital_static
    )

  if(NOT WIN32)
    set_target_properties(gnuradio-trellis_static
      PROPERTIES OUTPUT_NAME gnuradio-trellis)
  endif(NOT WIN32)

  install(TARGETS gnuradio-trellis_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "trellis_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
