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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_DIGITAL_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_digital_sources
    digital_impl_glfsr.cc
    digital_impl_mpsk_snr_est.cc
    digital_additive_scrambler_bb.cc
    digital_binary_slicer_fb.cc
    digital_bytes_to_syms.cc
    digital_clock_recovery_mm_cc.cc
    digital_clock_recovery_mm_ff.cc
    digital_cma_equalizer_cc.cc
    digital_constellation.cc
    digital_constellation_receiver_cb.cc
    digital_constellation_decoder_cb.cc
    digital_correlate_access_code_bb.cc
    digital_correlate_access_code_tag_bb.cc
    digital_costas_loop_cc.cc
    digital_cpmmod_bc.cc
    digital_crc32.cc
    digital_descrambler_bb.cc
    digital_diff_decoder_bb.cc
    digital_diff_encoder_bb.cc
    digital_diff_phasor_cc.cc
    digital_fll_band_edge_cc.cc
    digital_framer_sink_1.cc
    digital_glfsr_source_b.cc
    digital_glfsr_source_f.cc
    digital_gmskmod_bc.cc
    digital_lms_dd_equalizer_cc.cc
    digital_kurtotic_equalizer_cc.cc
    digital_map_bb.cc
    digital_mpsk_receiver_cc.cc
    digital_mpsk_snr_est_cc.cc
    digital_ofdm_cyclic_prefixer.cc
    digital_ofdm_frame_acquisition.cc
    digital_ofdm_frame_sink.cc
    digital_ofdm_insert_preamble.cc
    digital_ofdm_mapper_bcv.cc
    digital_ofdm_sampler.cc
    digital_packet_sink.cc
    digital_pfb_clock_sync_ccf.cc
    digital_pfb_clock_sync_fff.cc
    digital_pn_correlator_cc.cc
    digital_probe_density_b.cc
    digital_probe_mpsk_snr_est_c.cc
    digital_scrambler_bb.cc
    digital_simple_framer.cc
)

list(APPEND digital_libs
    gnuradio-core
    ${Boost_LIBRARIES}
)

add_library(gnuradio-digital SHARED ${gr_digital_sources})
target_link_libraries(gnuradio-digital ${digital_libs})
GR_LIBRARY_FOO(gnuradio-digital RUNTIME_COMPONENT "digital_runtime" DEVEL_COMPONENT "digital_devel")
