
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

void bind_alaw_decode_bs(py::module&);
void bind_alaw_encode_sb(py::module&);
#ifdef LIBCODEC2_FOUND
void bind_codec2(py::module&);
void bind_codec2_decode_ps(py::module&);
void bind_codec2_encode_sp(py::module&);
#endif
void bind_cvsd_decode_bs(py::module&);
void bind_cvsd_encode_sb(py::module&);
#ifdef LIBCODEC2_HAS_FREEDV_API
void bind_freedv_api(py::module&);
void bind_freedv_rx_ss(py::module&);
void bind_freedv_tx_ss(py::module&);
#endif
void bind_g721_decode_bs(py::module&);
void bind_g721_encode_sb(py::module&);
void bind_g723_24_decode_bs(py::module&);
void bind_g723_24_encode_sb(py::module&);
void bind_g723_40_decode_bs(py::module&);
void bind_g723_40_encode_sb(py::module&);
#ifdef LIBGSM_FOUND
void bind_gsm_fr_decode_ps(py::module&);
void bind_gsm_fr_encode_sp(py::module&);
#endif
void bind_ulaw_decode_bs(py::module&);
void bind_ulaw_encode_sb(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(vocoder_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_alaw_decode_bs(m);
    bind_alaw_encode_sb(m);
#ifdef LIBCODEC2_FOUND
    bind_codec2(m);
    bind_codec2_decode_ps(m);
    bind_codec2_encode_sp(m);
#endif
    bind_cvsd_decode_bs(m);
    bind_cvsd_encode_sb(m);
#ifdef LIBCODEC2_HAS_FREEDV_API
    bind_freedv_api(m);
    bind_freedv_rx_ss(m);
    bind_freedv_tx_ss(m);
#endif
    bind_g721_decode_bs(m);
    bind_g721_encode_sb(m);
    bind_g723_24_decode_bs(m);
    bind_g723_24_encode_sb(m);
    bind_g723_40_decode_bs(m);
    bind_g723_40_encode_sb(m);
#ifdef LIBGSM_FOUND
    bind_gsm_fr_decode_ps(m);
    bind_gsm_fr_encode_sp(m);
#endif
    bind_ulaw_decode_bs(m);
    bind_ulaw_encode_sb(m);
}
