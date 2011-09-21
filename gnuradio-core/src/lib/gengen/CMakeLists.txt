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
# generate the python helper script which calls into the build utils
########################################################################
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py "
#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_CORE_PYTHONPATH}')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, out = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict(name, sig)
        build_utils.expand_template(d, out)

")

########################################################################
# generation helper macro to generate various files from template
########################################################################
MACRO(expand_h_cc_i root)

    FOREACH(ext cc h i)
        #make a list of all the generated files
        UNSET(expanded_files_${ext})
        FOREACH(sig ${ARGN})
            STRING(REGEX REPLACE "X+" ${sig} name ${root})
            LIST(APPEND expanded_files_${ext} ${CMAKE_CURRENT_BINARY_DIR}/${name}.${ext})
        ENDFOREACH(sig)

        #create a command to generate the files
        ADD_CUSTOM_COMMAND(
            OUTPUT ${expanded_files_${ext}}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.${ext}.t
            COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
                ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
                ${root} ${root}.${ext}.t ${ARGN}
        )
    ENDFOREACH(ext)

    # install rules for the generated cc, h, and i files
    LIST(APPEND generated_gengen_sources ${expanded_files_cc})
    LIST(APPEND generated_gengen_includes ${expanded_files_h})
    LIST(APPEND generated_gengen_swigs ${expanded_files_i})

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
# Create the master gengen swig include files
########################################################################
FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i
    "// This file is machine generated.  All edits will be overwritten\n"
)

FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "%{\n")
FOREACH(swig_file ${generated_gengen_swigs})
    GET_FILENAME_COMPONENT(name ${swig_file} NAME)
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "    #include<${name}.h>\n")
ENDFOREACH(swig_file)
FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "%}\n")

FOREACH(swig_file ${generated_gengen_swigs})
    GET_FILENAME_COMPONENT(name ${swig_file} NAME)
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i "%include<${name}.h>\n")
ENDFOREACH(swig_file)

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
