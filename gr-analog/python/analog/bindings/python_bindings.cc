
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

void bind_agc(py::module&);
void bind_agc2(py::module&);
void bind_agc2_cc(py::module&);
void bind_agc2_ff(py::module&);
void bind_agc3_cc(py::module&);
void bind_agc_cc(py::module&);
void bind_agc_ff(py::module&);
void bind_cpfsk_bc(py::module&);
void bind_cpm(py::module&);
void bind_ctcss_squelch_ff(py::module&);
void bind_dpll_bb(py::module&);
void bind_fastnoise_source(py::module&);
void bind_feedforward_agc_cc(py::module&);
void bind_fmdet_cf(py::module&);
void bind_frequency_modulator_fc(py::module&);
void bind_noise_source(py::module&);
void bind_noise_type(py::module&);
void bind_phase_modulator_fc(py::module&);
void bind_pll_carriertracking_cc(py::module&);
void bind_pll_freqdet_cf(py::module&);
void bind_pll_refout_cc(py::module&);
void bind_probe_avg_mag_sqrd_c(py::module&);
void bind_probe_avg_mag_sqrd_cf(py::module&);
void bind_probe_avg_mag_sqrd_f(py::module&);
void bind_pwr_squelch_cc(py::module&);
void bind_pwr_squelch_ff(py::module&);
void bind_quadrature_demod_cf(py::module&);
void bind_rail_ff(py::module&);
void bind_random_uniform_source(py::module&);
void bind_sig_source(py::module&);
void bind_sig_source_waveform(py::module&);
void bind_simple_squelch_cc(py::module&);
void bind_squelch_base_cc(py::module&);
void bind_squelch_base_ff(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(analog_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");
    py::module::import("gnuradio.blocks");

    bind_agc(m);
    // bind_agc2(m);
    bind_agc2_cc(m);
    bind_agc2_ff(m);
    bind_agc3_cc(m);
    bind_agc_cc(m);
    bind_agc_ff(m);
    bind_cpfsk_bc(m);
    bind_cpm(m);
    bind_dpll_bb(m);
    bind_fastnoise_source(m);
    bind_feedforward_agc_cc(m);
    bind_fmdet_cf(m);
    bind_frequency_modulator_fc(m);
    bind_noise_source(m);
    bind_noise_type(m);
    bind_phase_modulator_fc(m);
    bind_pll_carriertracking_cc(m);
    bind_pll_freqdet_cf(m);
    bind_pll_refout_cc(m);
    bind_probe_avg_mag_sqrd_c(m);
    bind_probe_avg_mag_sqrd_cf(m);
    bind_probe_avg_mag_sqrd_f(m);
    bind_quadrature_demod_cf(m);
    bind_rail_ff(m);
    bind_random_uniform_source(m);
    bind_sig_source(m);
    bind_sig_source_waveform(m);
    bind_simple_squelch_cc(m);
    bind_squelch_base_cc(m);
    bind_squelch_base_ff(m);
    bind_ctcss_squelch_ff(m);
    bind_pwr_squelch_cc(m);
    bind_pwr_squelch_ff(m);
}
