
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

void bind_dc_blocker_cc(py::module&);
void bind_dc_blocker_ff(py::module&);
void bind_fft_filter(py::module&);
void bind_fft_filter_ccc(py::module&);
void bind_fft_filter_ccf(py::module&);
void bind_fft_filter_fff(py::module&);
void bind_filter_delay_fc(py::module&);
void bind_filterbank(py::module&);
void bind_filterbank_vcvcf(py::module&);
void bind_fir_filter(py::module&);
void bind_fir_filter_blk(py::module&);
void bind_fir_filter_with_buffer(py::module&);
void bind_firdes(py::module&);
void bind_freq_xlating_fir_filter(py::module&);
void bind_hilbert_fc(py::module&);
// void bind_iir_filter(py::module&);
void bind_iir_filter_ccc(py::module&);
void bind_iir_filter_ccd(py::module&);
void bind_iir_filter_ccf(py::module&);
void bind_iir_filter_ccz(py::module&);
void bind_iir_filter_ffd(py::module&);
// void bind_interp_differentiator_taps(py::module&);
void bind_interp_fir_filter(py::module&);
// void bind_interpolator_taps(py::module&);
void bind_ival_decimator(py::module&);
void bind_mmse_fir_interpolator_cc(py::module&);
void bind_mmse_fir_interpolator_ff(py::module&);
void bind_mmse_interp_differentiator_cc(py::module&);
void bind_mmse_interp_differentiator_ff(py::module&);
void bind_mmse_resampler_cc(py::module&);
void bind_mmse_resampler_ff(py::module&);
void bind_pfb_arb_resampler(py::module&);
void bind_pfb_arb_resampler_ccc(py::module&);
void bind_pfb_arb_resampler_ccf(py::module&);
void bind_pfb_arb_resampler_fff(py::module&);
void bind_pfb_channelizer_ccf(py::module&);
void bind_pfb_decimator_ccf(py::module&);
void bind_pfb_interpolator_ccf(py::module&);
void bind_pfb_synthesizer_ccf(py::module&);
void bind_pm_remez(py::module&);
void bind_polyphase_filterbank(py::module&);
void bind_rational_resampler(py::module&);
void bind_single_pole_iir(py::module&);
void bind_single_pole_iir_filter_cc(py::module&);
void bind_single_pole_iir_filter_ff(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(filter_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_dc_blocker_cc(m);
    bind_dc_blocker_ff(m);
    bind_fft_filter(m);
    bind_fft_filter_ccc(m);
    bind_fft_filter_ccf(m);
    bind_fft_filter_fff(m);
    bind_filter_delay_fc(m);
    bind_filterbank(m);
    bind_filterbank_vcvcf(m);
    bind_fir_filter(m);
    bind_fir_filter_blk(m);
    bind_fir_filter_with_buffer(m);
    bind_firdes(m);
    bind_freq_xlating_fir_filter(m);
    bind_hilbert_fc(m);
    // bind_iir_filter(m);
    bind_iir_filter_ccc(m);
    bind_iir_filter_ccd(m);
    bind_iir_filter_ccf(m);
    bind_iir_filter_ccz(m);
    bind_iir_filter_ffd(m);
    // bind_interp_differentiator_taps(m);
    bind_interp_fir_filter(m);
    // bind_interpolator_taps(m);
    bind_ival_decimator(m);
    bind_mmse_fir_interpolator_cc(m);
    bind_mmse_fir_interpolator_ff(m);
    bind_mmse_interp_differentiator_cc(m);
    bind_mmse_interp_differentiator_ff(m);
    bind_mmse_resampler_cc(m);
    bind_mmse_resampler_ff(m);
    bind_pfb_arb_resampler(m);
    bind_pfb_arb_resampler_ccc(m);
    bind_pfb_arb_resampler_ccf(m);
    bind_pfb_arb_resampler_fff(m);
    bind_pfb_channelizer_ccf(m);
    bind_pfb_decimator_ccf(m);
    bind_pfb_interpolator_ccf(m);
    bind_pfb_synthesizer_ccf(m);
    bind_pm_remez(m);
    bind_polyphase_filterbank(m);
    bind_rational_resampler(m);
    bind_single_pole_iir(m);
    bind_single_pole_iir_filter_cc(m);
    bind_single_pole_iir_filter_ff(m);
}
