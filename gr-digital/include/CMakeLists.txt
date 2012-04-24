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
########################################################################
expand_h(digital_chunks_to_symbols_XX         bf bc sf sc if ic)

add_custom_target(digital_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    digital_api.h
    digital_impl_glfsr.h
    digital_impl_mpsk_snr_est.h
    digital_additive_scrambler_bb.h
    digital_binary_slicer_fb.h
    digital_bytes_to_syms.h
    digital_clock_recovery_mm_cc.h
    digital_clock_recovery_mm_ff.h
    digital_cma_equalizer_cc.h
    digital_cpmmod_bc.h
    digital_constellation.h
    digital_constellation_receiver_cb.h
    digital_constellation_decoder_cb.h
    digital_correlate_access_code_bb.h
    digital_correlate_access_code_tag_bb.h
    digital_costas_loop_cc.h
    digital_crc32.h
    digital_descrambler_bb.h
    digital_diff_decoder_bb.h
    digital_diff_encoder_bb.h
    digital_diff_phasor_cc.h
    digital_framer_sink_1.h
    digital_fll_band_edge_cc.h
    digital_glfsr_source_b.h
    digital_glfsr_source_f.h
    digital_gmskmod_bc.h
    digital_lms_dd_equalizer_cc.h
    digital_kurtotic_equalizer_cc.h
    digital_map_bb.h
    digital_metric_type.h
    digital_mpsk_receiver_cc.h
    digital_mpsk_snr_est_cc.h
    digital_ofdm_cyclic_prefixer.h
    digital_ofdm_frame_acquisition.h
    digital_ofdm_frame_sink.h
    digital_ofdm_insert_preamble.h
    digital_ofdm_mapper_bcv.h
    digital_ofdm_sampler.h
    digital_packet_sink.h
    digital_pfb_clock_sync_ccf.h
    digital_pfb_clock_sync_fff.h
    digital_pn_correlator_cc.h
    digital_probe_density_b.h
    digital_probe_mpsk_snr_est_c.h
    digital_scrambler_bb.h
    digital_simple_framer.h
    digital_simple_framer_sync.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "digital_devel"
)

