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
include(GrPython)

########################################################################
# generate the python helper script which calls into the build utils
########################################################################
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
        d = build_utils.standard_dict(name, sig)
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
    list(APPEND generated_gengen_sources ${expanded_files_cc})
    list(APPEND generated_gengen_includes ${expanded_files_h})
    list(APPEND generated_gengen_swigs ${expanded_files_i})

endmacro(expand_h_cc_i)

########################################################################
# Invoke macro to generate various sources
########################################################################
expand_h_cc_i(gr_vector_source_X   b s i f c)
expand_h_cc_i(gr_vector_insert_X   b)
expand_h_cc_i(gr_vector_sink_X     b s i f c)
expand_h_cc_i(gr_noise_source_X      s i f c)
expand_h_cc_i(gr_sig_source_X        s i f c)
expand_h_cc_i(gr_probe_signal_X    b s i f c)
expand_h_cc_i(gr_probe_signal_vX    b s i f c)

expand_h_cc_i(gr_add_const_XX           ss ii ff cc sf)
expand_h_cc_i(gr_multiply_const_XX      ss ii)
expand_h_cc_i(gr_add_XX                 ss ii cc)
expand_h_cc_i(gr_sub_XX                 ss ii ff cc)
expand_h_cc_i(gr_multiply_XX            ss ii)
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

add_custom_target(gengen_generated DEPENDS
    ${generated_gengen_includes}
    ${generated_gengen_swigs}
)

########################################################################
# Create the master gengen swig include files
########################################################################
set(generated_index ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i.in)
file(WRITE ${generated_index} "
//
// This file is machine generated.  All edits will be overwritten
//
")

file(APPEND ${generated_index} "%{\n")
foreach(swig_file ${generated_gengen_swigs})
    get_filename_component(name ${swig_file} NAME_WE)
    file(APPEND ${generated_index} "#include<${name}.h>\n")
endforeach(swig_file)
file(APPEND ${generated_index} "%}\n")

foreach(swig_file ${generated_gengen_swigs})
    get_filename_component(name ${swig_file} NAME)
    file(APPEND ${generated_index} "%include<${name}>\n")
endforeach(swig_file)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${generated_index} ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i
)

########################################################################
# Handle the generated sources + a few non-generated ones
########################################################################
list(APPEND gnuradio_core_sources
    ${generated_gengen_sources}
)

install(FILES
    ${generated_gengen_includes}
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_endianness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_noise_type.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sig_source_waveform.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

if(ENABLE_PYTHON)
    install(FILES
        ${generated_gengen_swigs}
        ${CMAKE_CURRENT_SOURCE_DIR}/gr_endianness.i
        ${CMAKE_CURRENT_SOURCE_DIR}/gengen.i
        ${CMAKE_CURRENT_BINARY_DIR}/gengen_generated.i
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
        COMPONENT "core_swig"
    )
endif(ENABLE_PYTHON)
