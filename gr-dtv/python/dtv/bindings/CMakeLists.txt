include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    dtv_python_files
    atsc_consts_python.cc
    atsc_deinterleaver_python.cc
    atsc_depad_python.cc
    atsc_derandomizer_python.cc
    atsc_equalizer_python.cc
    atsc_field_sync_mux_python.cc
    atsc_fpll_python.cc
    atsc_fs_checker_python.cc
    atsc_interleaver_python.cc
    atsc_pad_python.cc
    atsc_randomizer_python.cc
    atsc_rs_decoder_python.cc
    atsc_rs_encoder_python.cc
    atsc_sync_python.cc
    atsc_trellis_encoder_python.cc
    atsc_viterbi_decoder_python.cc
    catv_config_python.cc
    catv_frame_sync_enc_bb_python.cc
    catv_randomizer_bb_python.cc
    catv_reed_solomon_enc_bb_python.cc
    catv_transport_framing_enc_bb_python.cc
    catv_trellis_enc_bb_python.cc
    dvb_bbheader_bb_python.cc
    dvb_bbscrambler_bb_python.cc
    dvb_bch_bb_python.cc
    dvb_config_python.cc
    dvb_ldpc_bb_python.cc
    dvbs2_config_python.cc
    dvbs2_interleaver_bb_python.cc
    dvbs2_modulator_bc_python.cc
    dvbs2_physical_cc_python.cc
    dvbt2_cellinterleaver_cc_python.cc
    dvbt2_config_python.cc
    dvbt2_framemapper_cc_python.cc
    dvbt2_freqinterleaver_cc_python.cc
    dvbt2_interleaver_bb_python.cc
    dvbt2_miso_cc_python.cc
    dvbt2_modulator_bc_python.cc
    dvbt2_p1insertion_cc_python.cc
    dvbt2_paprtr_cc_python.cc
    dvbt2_pilotgenerator_cc_python.cc
    dvbt_bit_inner_deinterleaver_python.cc
    dvbt_bit_inner_interleaver_python.cc
    dvbt_config_python.cc
    dvbt_convolutional_deinterleaver_python.cc
    dvbt_convolutional_interleaver_python.cc
    dvbt_demap_python.cc
    dvbt_demod_reference_signals_python.cc
    dvbt_energy_descramble_python.cc
    dvbt_energy_dispersal_python.cc
    dvbt_inner_coder_python.cc
    dvbt_map_python.cc
    dvbt_ofdm_sym_acquisition_python.cc
    dvbt_reed_solomon_dec_python.cc
    dvbt_reed_solomon_enc_python.cc
    dvbt_reference_signals_python.cc
    dvbt_symbol_inner_interleaver_python.cc
    dvbt_viterbi_decoder_python.cc
    python_bindings.cc)

gr_pybind_make_check_hash(dtv ../../.. gr::dtv "${dtv_python_files}")

install(
    TARGETS dtv_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/dtv
    COMPONENT pythonapi)
