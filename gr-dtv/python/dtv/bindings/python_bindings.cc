
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

void bind_atsc_consts(py::module&);
void bind_atsc_deinterleaver(py::module&);
void bind_atsc_depad(py::module&);
void bind_atsc_derandomizer(py::module&);
void bind_atsc_equalizer(py::module&);
void bind_atsc_field_sync_mux(py::module&);
void bind_atsc_fpll(py::module&);
void bind_atsc_fs_checker(py::module&);
void bind_atsc_interleaver(py::module&);
void bind_atsc_pad(py::module&);
void bind_atsc_randomizer(py::module&);
void bind_atsc_rs_decoder(py::module&);
void bind_atsc_rs_encoder(py::module&);
void bind_atsc_sync(py::module&);
void bind_atsc_trellis_encoder(py::module&);
void bind_atsc_viterbi_decoder(py::module&);
void bind_catv_config(py::module&);
void bind_catv_frame_sync_enc_bb(py::module&);
void bind_catv_randomizer_bb(py::module&);
void bind_catv_reed_solomon_enc_bb(py::module&);
void bind_catv_transport_framing_enc_bb(py::module&);
void bind_catv_trellis_enc_bb(py::module&);
void bind_dvb_bbheader_bb(py::module&);
void bind_dvb_bbscrambler_bb(py::module&);
void bind_dvb_bch_bb(py::module&);
void bind_dvb_config(py::module&);
void bind_dvb_ldpc_bb(py::module&);
void bind_dvbs2_config(py::module&);
void bind_dvbs2_interleaver_bb(py::module&);
void bind_dvbs2_modulator_bc(py::module&);
void bind_dvbs2_physical_cc(py::module&);
void bind_dvbt2_cellinterleaver_cc(py::module&);
void bind_dvbt2_config(py::module&);
void bind_dvbt2_framemapper_cc(py::module&);
void bind_dvbt2_freqinterleaver_cc(py::module&);
void bind_dvbt2_interleaver_bb(py::module&);
void bind_dvbt2_miso_cc(py::module&);
void bind_dvbt2_modulator_bc(py::module&);
void bind_dvbt2_p1insertion_cc(py::module&);
void bind_dvbt2_paprtr_cc(py::module&);
void bind_dvbt2_pilotgenerator_cc(py::module&);
void bind_dvbt_bit_inner_deinterleaver(py::module&);
void bind_dvbt_bit_inner_interleaver(py::module&);
void bind_dvbt_config(py::module&);
void bind_dvbt_convolutional_deinterleaver(py::module&);
void bind_dvbt_convolutional_interleaver(py::module&);
void bind_dvbt_demap(py::module&);
void bind_dvbt_demod_reference_signals(py::module&);
void bind_dvbt_energy_descramble(py::module&);
void bind_dvbt_energy_dispersal(py::module&);
void bind_dvbt_inner_coder(py::module&);
void bind_dvbt_map(py::module&);
void bind_dvbt_ofdm_sym_acquisition(py::module&);
void bind_dvbt_reed_solomon_dec(py::module&);
void bind_dvbt_reed_solomon_enc(py::module&);
void bind_dvbt_reference_signals(py::module&);
void bind_dvbt_symbol_inner_interleaver(py::module&);
void bind_dvbt_viterbi_decoder(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(dtv_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_atsc_consts(m);
    bind_atsc_deinterleaver(m);
    bind_atsc_depad(m);
    bind_atsc_derandomizer(m);
    bind_atsc_equalizer(m);
    bind_atsc_field_sync_mux(m);
    bind_atsc_fpll(m);
    bind_atsc_fs_checker(m);
    bind_atsc_interleaver(m);
    bind_atsc_pad(m);
    bind_atsc_randomizer(m);
    bind_atsc_rs_decoder(m);
    bind_atsc_rs_encoder(m);
    bind_atsc_sync(m);
    bind_atsc_trellis_encoder(m);
    bind_atsc_viterbi_decoder(m);
    bind_catv_config(m);
    bind_catv_frame_sync_enc_bb(m);
    bind_catv_randomizer_bb(m);
    bind_catv_reed_solomon_enc_bb(m);
    bind_catv_transport_framing_enc_bb(m);
    bind_catv_trellis_enc_bb(m);
    bind_dvb_bbheader_bb(m);
    bind_dvb_bbscrambler_bb(m);
    bind_dvb_bch_bb(m);
    bind_dvb_config(m);
    bind_dvb_ldpc_bb(m);
    bind_dvbs2_config(m);
    bind_dvbs2_interleaver_bb(m);
    bind_dvbs2_modulator_bc(m);
    bind_dvbs2_physical_cc(m);
    bind_dvbt2_cellinterleaver_cc(m);
    bind_dvbt2_config(m);
    bind_dvbt2_framemapper_cc(m);
    bind_dvbt2_freqinterleaver_cc(m);
    bind_dvbt2_interleaver_bb(m);
    bind_dvbt2_miso_cc(m);
    bind_dvbt2_modulator_bc(m);
    bind_dvbt2_p1insertion_cc(m);
    bind_dvbt2_paprtr_cc(m);
    bind_dvbt2_pilotgenerator_cc(m);
    bind_dvbt_bit_inner_deinterleaver(m);
    bind_dvbt_bit_inner_interleaver(m);
    bind_dvbt_config(m);
    bind_dvbt_convolutional_deinterleaver(m);
    bind_dvbt_convolutional_interleaver(m);
    bind_dvbt_demap(m);
    bind_dvbt_demod_reference_signals(m);
    bind_dvbt_energy_descramble(m);
    bind_dvbt_energy_dispersal(m);
    bind_dvbt_inner_coder(m);
    bind_dvbt_map(m);
    bind_dvbt_ofdm_sym_acquisition(m);
    bind_dvbt_reed_solomon_dec(m);
    bind_dvbt_reed_solomon_enc(m);
    bind_dvbt_reference_signals(m);
    bind_dvbt_symbol_inner_interleaver(m);
    bind_dvbt_viterbi_decoder(m);
}
