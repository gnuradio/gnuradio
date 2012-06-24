# Copyright 2011,2012 Free Software Foundation, Inc.
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
        d = build_utils.standard_dict(name, sig, 'digital')
        build_utils.expand_template(d, inp)

")

macro(expand_i root)
  # make a list of the .i generated files
  unset(expanded_files_i)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_i ${CMAKE_CURRENT_BINARY_DIR}/${name}.i)
  endforeach(sig)

  #create a command to generate the .i files
  add_custom_command(
    OUTPUT ${expanded_files_i}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.i.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.i.t ${ARGN}
    )

  # Lists of generated i files
  list(APPEND generated_swigs ${expanded_files_i})
endmacro(expand_i)


########################################################################
# Invoke macro to generate various sources
########################################################################
expand_i(digital_chunks_to_symbols_XX         bf bc sf sc if ic)

add_custom_target(digital_generated_swigs DEPENDS
    ${generated_swigs}
)

########################################################################
# Setup swig generation
########################################################################
include(GrSwig)

########################################################################
# Create the master gengen swig include files
########################################################################
set(generated_index ${CMAKE_CURRENT_BINARY_DIR}/digital_generated.i.in)
file(WRITE ${generated_index} "
//
// This file is machine generated.  All edits will be overwritten
//
")

file(APPEND ${generated_index} "%include \"gnuradio.i\"\n\n")
file(APPEND ${generated_index} "%{\n")

foreach(swig_file ${generated_swigs})
    get_filename_component(name ${swig_file} NAME_WE)
    file(APPEND ${generated_index} "#include<${name}.h>\n")
endforeach(swig_file)
file(APPEND ${generated_index} "%}\n")

foreach(swig_file ${generated_swigs})
    get_filename_component(name ${swig_file} NAME)
    file(APPEND ${generated_index} "%include<${name}>\n")
endforeach(swig_file)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${generated_index} ${CMAKE_CURRENT_BINARY_DIR}/digital_generated.i
)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_DIGITAL_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

# Setup swig docs to depend on includes and pull in from build directory
set(GR_SWIG_LIBRARIES gnuradio-digital)
set(GR_SWIG_TARGET_DEPS digital_generated_includes)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/digital_swig_doc.i)
set(GR_SWIG_DOC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  ${CMAKE_CURRENT_BINARY_DIR}/../include)
GR_SWIG_MAKE(digital_swig digital_swig.i)
GR_SWIG_INSTALL(
    TARGETS digital_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital
    COMPONENT "digital_python"
)

install(
    FILES
    digital_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/digital_swig_doc.i
    ${CMAKE_CURRENT_BINARY_DIR}/digital_generated.i
    ${generated_swigs}
    digital_additive_scrambler_bb.i
    digital_binary_slicer_fb.i
    digital_bytes_to_syms.i
    digital_clock_recovery_mm_cc.i
    digital_clock_recovery_mm_ff.i
    digital_cma_equalizer_cc.i
    digital_constellation.i
    digital_constellation_receiver_cb.i
    digital_constellation_decoder_cb.i
    digital_correlate_access_code_bb.i
    digital_correlate_access_code_tag_bb.i
    digital_costas_loop_cc.i
    digital_cpmmod_bc.i
    digital_crc32.i
    digital_descrambler_bb.i
    digital_diff_decoder_bb.i
    digital_diff_encoder_bb.i
    digital_diff_phasor_cc.i
    digital_fll_band_edge_cc.i
    digital_framer_sink_1.i
    digital_glfsr_source_b.i
    digital_glfsr_source_f.i
    digital_gmskmod_bc.i
    digital_lms_dd_equalizer_cc.i
    digital_kurtotic_equalizer_cc.i
    digital_map_bb.i
    digital_mpsk_receiver_cc.i
    digital_mpsk_snr_est_cc.i
    digital_ofdm_cyclic_prefixer.i
    digital_ofdm_frame_acquisition.i
    digital_ofdm_frame_sink.i
    digital_ofdm_insert_preamble.i
    digital_ofdm_mapper_bcv.i
    digital_ofdm_sampler.i
    digital_packet_sink.i
    digital_pfb_clock_sync_ccf.i
    digital_pfb_clock_sync_fff.i
    digital_pn_correlator_cc.i
    digital_probe_density_b.i
    digital_probe_mpsk_snr_est_c.i
    digital_scrambler_bb.i
    digital_simple_framer.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "digital_swig"
)
