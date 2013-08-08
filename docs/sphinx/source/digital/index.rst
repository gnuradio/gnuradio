gnuradio.digital
================

.. automodule:: gnuradio.digital

Signal Processing Blocks
------------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.fll_band_edge_cc
   gnuradio.digital.kurtotic_equalizer_cc
   gnuradio.digital.lms_dd_equalizer_cc
   gnuradio.digital.mpsk_receiver_cc
   gnuradio.digital.mpsk_snr_est_cc
   gnuradio.digital.clock_recovery_mm_cc
   gnuradio.digital.clock_recovery_mm_ff
   gnuradio.digital.constellation_decoder_cb
   gnuradio.digital.constellation_receiver_cb
   gnuradio.digital.correlate_access_code_bb
   gnuradio.digital.costas_loop_cc
   gnuradio.digital.cma_equalizer_cc
   gnuradio.digital.binary_slicer_fb
   gnuradio.digital.gmskmod_bc
   gnuradio.digital.probe_mpsk_snr_est_c
   gnuradio.digital.cpmmod_bc
   gnuradio.digital.glfsr_source_b
   gnuradio.digital.glfsr_source_f
   gnuradio.digital.framer_sink_1
   gnuradio.digital.additive_scrambler_bb
   gnuradio.digital.descrambler_bb
   gnuradio.digital.diff_decoder_bb
   gnuradio.digital.diff_encoder_bb
   gnuradio.digital.map_bb
   gnuradio.digital.scrambler_bb
   gnuradio.digital.pn_correlator_cc
   gnuradio.digital.simple_framer
   gnuradio.digital.chunks_to_symbols_bc
   gnuradio.digital.chunks_to_symbols_bf
   gnuradio.digital.chunks_to_symbols_ic
   gnuradio.digital.chunks_to_symbols_if
   gnuradio.digital.chunks_to_symbols_sc
   gnuradio.digital.chunks_to_symbols_sf
   gnuradio.digital.generic_demod
   gnuradio.digital.generic_mod
   gnuradio.digital.bpsk.dbpsk_demod
   gnuradio.digital.bpsk.dbpsk_mod
   gnuradio.digital.qpsk.dqpsk_demod
   gnuradio.digital.qpsk.dqpsk_mod
   gnuradio.digital.gmsk.gmsk_demod
   gnuradio.digital.gmsk.gmsk_mod
   gnuradio.digital.bpsk.bpsk_demod
   gnuradio.digital.bpsk.bpsk_mod
   gnuradio.digital.psk.psk_demod
   gnuradio.digital.psk.psk_mod
   gnuradio.digital.qam.qam_demod
   gnuradio.digital.qam.qam_mod
   gnuradio.digital.qpsk.qpsk_demod
   gnuradio.digital.qpsk.qpsk_mod
   gnuradio.digital.cpm.cpm_mod
   gnuradio.digital.pkt.mod_pkts
   gnuradio.digital.pkt.demod_pkts
   gnuradio.digital.ofdm_cyclic_prefixer
   gnuradio.digital.ofdm_frame_acquisition
   gnuradio.digital.ofdm_frame_sink
   gnuradio.digital.ofdm_insert_preamble
   gnuradio.digital.ofdm_mapper_bcv
   gnuradio.digital.ofdm_mod
   gnuradio.digital.ofdm_demod
   gnuradio.digital.ofdm_receiver
   gnuradio.digital.ofdm_sampler
   gnuradio.digital.ofdm_sync_fixed
   gnuradio.digital.ofdm_sync_ml
   gnuradio.digital.ofdm_sync_pn
   gnuradio.digital.ofdm_sync_pnac

Constellations
--------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.constellation_8psk
   gnuradio.digital.constellation_bpsk
   gnuradio.digital.constellation_calcdist
   gnuradio.digital.constellation_dqpsk
   gnuradio.digital.constellation_psk
   gnuradio.digital.constellation_qpsk
   gnuradio.digital.constellation_qpsk
   gnuradio.digital.constellation_rect
   gnuradio.digital.qpsk.qpsk_constellation
   gnuradio.digital.psk.psk_constellation
   gnuradio.digital.qam.qam_constellation

Modulation Utilties
-------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.utils.gray_code.gray_code
   gnuradio.digital.modulation_utils.add_type_1_constellation
   gnuradio.digital.modulation_utils.add_type_1_demod
   gnuradio.digital.modulation_utils.add_type_1_mod
   gnuradio.digital.modulation_utils.type_1_constellations
   gnuradio.digital.modulation_utils.type_1_demods
   gnuradio.digital.modulation_utils.type_1_mods

.. gnuradio.digital.utils.mod_codes.GRAY_CODE
.. gnuradio.digital.utils.mod_codes.NO_CODE

Packet Utilities
----------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.packet_utils.conv_1_0_string_to_packed_binary_string
   gnuradio.digital.packet_utils.conv_packed_binary_string_to_1_0_string
   gnuradio.digital.packet_utils.default_access_code
   gnuradio.digital.packet_utils.dewhiten
   gnuradio.digital.packet_utils.is_1_0_string
   gnuradio.digital.packet_utils.make_header
   gnuradio.digital.packet_utils.make_packet
   gnuradio.digital.packet_utils.preamble
   gnuradio.digital.packet_utils.random_mask_tuple
   gnuradio.digital.packet_utils.random_mask_vec8
   gnuradio.digital.packet_utils.string_to_hex_list
   gnuradio.digital.packet_utils.unmake_packet
   gnuradio.digital.packet_utils.whiten
   gnuradio.digital.crc.check_crc32
   gnuradio.digital.crc.gen_and_append_crc32

OFDM Packet Utilities
---------------------

.. autosummary::
   :nosignatures:

   gnuradio.digital.ofdm_packet_utils.conv_1_0_string_to_packed_binary_string
   gnuradio.digital.ofdm_packet_utils.conv_packed_binary_string_to_1_0_string
   gnuradio.digital.ofdm_packet_utils.dewhiten
   gnuradio.digital.ofdm_packet_utils.is_1_0_string
   gnuradio.digital.ofdm_packet_utils.make_header
   gnuradio.digital.ofdm_packet_utils.make_packet
   gnuradio.digital.ofdm_packet_utils.random_mask_tuple
   gnuradio.digital.ofdm_packet_utils.random_mask_vec8
   gnuradio.digital.ofdm_packet_utils.string_to_hex_list
   gnuradio.digital.ofdm_packet_utils.unmake_packet
   gnuradio.digital.ofdm_packet_utils.whiten
