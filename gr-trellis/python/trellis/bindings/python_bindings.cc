
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

// void bind_base(py::module&);
// void bind_calc_metric(py::module&);
void bind_constellation_metrics_cf(py::module&);
// void bind_core_algorithms(py::module&);
void bind_encoder(py::module&);
void bind_fsm(py::module&);
void bind_interleaver(py::module&);
void bind_metrics(py::module&);
void bind_pccc_decoder_blk(py::module&);
void bind_pccc_decoder_combined_blk(py::module&);
void bind_pccc_encoder(py::module&);
void bind_permutation(py::module&);
void bind_sccc_decoder_blk(py::module&);
void bind_sccc_decoder_combined_blk(py::module&);
void bind_sccc_encoder(py::module&);
void bind_siso_combined_f(py::module&);
void bind_siso_f(py::module&);
void bind_siso_type(py::module&);
void bind_viterbi(py::module&);
void bind_viterbi_combined(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(trellis_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    // bind_base(m);
    // bind_calc_metric(m);
    bind_constellation_metrics_cf(m);
    // bind_core_algorithms(m);
    bind_encoder(m);
    bind_fsm(m);
    bind_interleaver(m);
    bind_metrics(m);
    bind_pccc_decoder_blk(m);
    bind_pccc_decoder_combined_blk(m);
    bind_pccc_encoder(m);
    bind_permutation(m);
    bind_sccc_decoder_blk(m);
    bind_sccc_decoder_combined_blk(m);
    bind_sccc_encoder(m);
    bind_siso_combined_f(m);
    bind_siso_f(m);
    bind_siso_type(m);
    bind_viterbi(m);
    bind_viterbi_combined(m);
}
