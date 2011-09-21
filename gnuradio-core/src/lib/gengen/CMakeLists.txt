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
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i
    "// This file is machine generated.  All edits will be overwritten\n"
)

MACRO(expand_h_cc_i_sig root sig)
    STRING(REGEX REPLACE "X+" ${sig} name ${root})
    FOREACH(ext cc h i)
        ADD_CUSTOM_COMMAND(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.${ext}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.${ext}.t
            COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} -c
\"import os, sys\; sys.path.append('${GR_CORE_PYTHONPATH}')\; os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'\; import build_utils\; build_utils.expand_template(build_utils.standard_dict('${name}', '${sig}'), '${root}.${ext}.t')\"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    ENDFOREACH(ext)

    # install rules for the generated cc, h, and i files
    LIST(APPEND generated_gengen_sources ${CMAKE_CURRENT_BINARY_DIR}/${name}.cc)
    LIST(APPEND generated_gengen_includes ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
    LIST(APPEND generated_gengen_swigs ${CMAKE_CURRENT_BINARY_DIR}/${name}.i)

    # append entries into the global swig include file
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "%{#include<${name}.h>%}\n")
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "%include<${name}.i>\n")

ENDMACRO(expand_h_cc_i_sig)

MACRO(expand_h_cc_i root)
    FOREACH(sig ${ARGN})
        expand_h_cc_i_sig(${root} ${sig})
    ENDFOREACH(sig)
ENDMACRO(expand_h_cc_i)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_h_cc_i(gr_vector_source_X   b s i f c)
expand_h_cc_i(gr_vector_sink_X     b s i f c)
expand_h_cc_i(gr_noise_source_X      s i f c)
expand_h_cc_i(gr_sig_source_X        s i f c)

expand_h_cc_i(gr_add_const_XX           ss ii ff cc sf)
expand_h_cc_i(gr_multiply_const_XX      ss ii ff cc)
expand_h_cc_i(gr_add_XX                 ss ii ff cc)
expand_h_cc_i(gr_sub_XX                 ss ii ff cc)
expand_h_cc_i(gr_multiply_XX            ss ii ff cc)
expand_h_cc_i(gr_divide_XX              ss ii ff cc)
expand_h_cc_i(gr_mute_XX                ss ii ff cc)
expand_h_cc_i(gr_add_const_vXX          ss ii ff cc)
expand_h_cc_i(gr_multiply_const_vXX     ss ii ff cc)
expand_h_cc_i(gr_integrate_XX           ss ii ff cc)
expand_h_cc_i(gr_moving_average_XX      ss ii ff cc)

expand_h_cc_i(gr_chunks_to_symbols_XX         bf bc sf sc if ic)
expand_h_cc_i(gr_unpacked_to_packed_XX        bb ss ii)
expand_h_cc_i(gr_packed_to_unpacked_XX        bb ss ii)
expand_h_cc_i(gr_xor_XX                       bb ss ii)
expand_h_cc_i(gr_and_XX                       bb ss ii)
expand_h_cc_i(gr_and_const_XX                 bb ss ii)
expand_h_cc_i(gr_or_XX                        bb ss ii)
expand_h_cc_i(gr_not_XX                       bb ss ii)
expand_h_cc_i(gr_sample_and_hold_XX           bb ss ii ff)
expand_h_cc_i(gr_argmax_XX                    fs is ss)
expand_h_cc_i(gr_max_XX                       ff ii ss)
expand_h_cc_i(gr_peak_detector_XX             fb ib sb)

ADD_CUSTOM_TARGET(gengen_generated ALL DEPENDS
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
