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
        d = build_utils.standard_dict2(name, sig, 'digital')
        build_utils.expand_template(d, inp)

")

macro(expand_h root)
  #make a list of all the generated files
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  
  #create a command to generate the files
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
    )

  #install rules for the generated h files
  list(APPEND generated_includes ${expanded_files_h})
endmacro(expand_h)

########################################################################
# Invoke macro to generate various sources
#######################################################################
expand_h(chunks_to_symbols_XX     bf bc sf sc if ic)

add_custom_target(digital_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${digital_generated_includes}
    api.h
    constellation.h
    crc32.h
    glfsr.h
    mpsk_snr_est.h
    simple_framer_sync.h
    additive_scrambler_bb.h
    binary_slicer_fb.h
    clock_recovery_mm_cc.h
    clock_recovery_mm_ff.h
    cma_equalizer_cc.h
    cpmmod_bc.h
    constellation_receiver_cb.h
    constellation_decoder_cb.h
    correlate_access_code_bb.h
    correlate_access_code_tag_bb.h
    costas_loop_cc.h
    descrambler_bb.h
    diff_decoder_bb.h
    diff_encoder_bb.h
    diff_phasor_cc.h
    framer_sink_1.h
    fll_band_edge_cc.h
    glfsr_source_b.h
    glfsr_source_f.h
    kurtotic_equalizer_cc.h
    lms_dd_equalizer_cc.h
    map_bb.h
    metric_type.h
    mpsk_receiver_cc.h
    mpsk_snr_est_cc.h
    ofdm_cyclic_prefixer.h
    ofdm_frame_acquisition.h
    ofdm_frame_sink.h
    ofdm_insert_preamble.h
    ofdm_mapper_bcv.h
    ofdm_sampler.h
    packet_sink.h
    pfb_clock_sync_ccf.h
    pfb_clock_sync_fff.h
    pn_correlator_cc.h
    probe_density_b.h
    probe_mpsk_snr_est_c.h
    scrambler_bb.h
    simple_framer.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/digital
    COMPONENT "digital_devel"
)

