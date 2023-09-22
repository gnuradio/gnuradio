
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

// void bind_alist(py::module&);
void bind_async_decoder(py::module&);
void bind_async_encoder(py::module&);
// void bind_awgn_bp(py::module&);
void bind_ber_bf(py::module&);
void bind_cc_common(py::module&);
void bind_cc_decoder(py::module&);
void bind_cc_encoder(py::module&);
void bind_ccsds_encoder(py::module&);
void bind_cldpc(py::module&);
void bind_conv_bit_corr_bb(py::module&);
void bind_decode_ccsds_27_fb(py::module&);
void bind_decoder(py::module&);
void bind_depuncture_bb(py::module&);
void bind_dummy_decoder(py::module&);
void bind_dummy_encoder(py::module&);
void bind_encode_ccsds_27_bb(py::module&);
void bind_encoder(py::module&);
void bind_generic_decoder(py::module&);
void bind_generic_encoder(py::module&);
void bind_gf2mat(py::module&);
void bind_gf2vec(py::module&);
#ifdef HAVE_GSL
void bind_fec_mtrx(py::module&);
void bind_ldpc_G_matrix(py::module&);
void bind_ldpc_H_matrix(py::module&);
void bind_ldpc_bit_flip_decoder(py::module&);
void bind_ldpc_gen_mtrx_encoder(py::module&);
void bind_ldpc_par_mtrx_encoder(py::module&);
#endif
void bind_ldpc_decoder(py::module&);
void bind_ldpc_encoder(py::module&);
void bind_maxstar(py::module&);
void bind_polar_common(py::module&);
void bind_polar_decoder_common(py::module&);
void bind_polar_decoder_sc(py::module&);
void bind_polar_decoder_sc_list(py::module&);
void bind_polar_decoder_sc_systematic(py::module&);
void bind_polar_encoder(py::module&);
void bind_polar_encoder_systematic(py::module&);
void bind_puncture_bb(py::module&);
void bind_puncture_ff(py::module&);
void bind_repetition_decoder(py::module&);
void bind_repetition_encoder(py::module&);
void bind_rs(py::module&);
void bind_tagged_decoder(py::module&);
void bind_tagged_encoder(py::module&);
// void bind_tpc_common(py::module&);
void bind_tpc_decoder(py::module&);
void bind_tpc_encoder(py::module&);
// void bind_viterbi(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(fec_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_generic_decoder(m);
    bind_generic_encoder(m);
    // bind_alist(m);
    bind_async_decoder(m);
    bind_async_encoder(m);
    // bind_awgn_bp(m);
    bind_ber_bf(m);
    bind_cc_common(m);
    bind_cc_decoder(m);
    bind_cc_encoder(m);
    bind_decoder(m);
    bind_encoder(m);
    bind_ccsds_encoder(m);
    // bind_cldpc(m);
    bind_conv_bit_corr_bb(m);
    bind_decode_ccsds_27_fb(m);
    bind_depuncture_bb(m);
    bind_dummy_decoder(m);
    bind_dummy_encoder(m);
    bind_encode_ccsds_27_bb(m);
    // bind_gf2mat(m);
    // bind_gf2vec(m);
#ifdef HAVE_GSL
    bind_fec_mtrx(m);
    bind_ldpc_G_matrix(m);
    bind_ldpc_H_matrix(m);
    bind_ldpc_bit_flip_decoder(m);
    bind_ldpc_gen_mtrx_encoder(m);
    bind_ldpc_par_mtrx_encoder(m);
#endif
    bind_ldpc_decoder(m);
    bind_ldpc_encoder(m);
    // bind_maxstar(m);
    bind_polar_common(m);
    bind_polar_decoder_common(m);
    bind_polar_decoder_sc(m);
    bind_polar_decoder_sc_list(m);
    bind_polar_decoder_sc_systematic(m);
    bind_polar_encoder(m);
    bind_polar_encoder_systematic(m);
    bind_puncture_bb(m);
    bind_puncture_ff(m);
    bind_repetition_decoder(m);
    bind_repetition_encoder(m);
    // bind_rs(m);
    bind_tagged_decoder(m);
    bind_tagged_encoder(m);
    // bind_tpc_common(m);
    bind_tpc_decoder(m);
    bind_tpc_encoder(m);
    // bind_viterbi(m);
}
