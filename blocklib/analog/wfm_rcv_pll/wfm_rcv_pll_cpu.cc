/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "wfm_rcv_pll_cpu.h"
#include "wfm_rcv_pll_cpu_gen.h"

#include <gnuradio/analog/fm_deemph.h>
#include <gnuradio/analog/pll_refout.h>
#include <gnuradio/analog/quadrature_demod.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/kernel/filter/firdes.h>
#include <gnuradio/kernel/math/math.h>
#include <gnuradio/math/add.h>
#include <gnuradio/math/complex_to_imag.h>
#include <gnuradio/math/multiply.h>
#include <gnuradio/math/sub.h>
#include <gnuradio/streamops/delay.h>

using namespace gr::kernel::filter;
using namespace gr::kernel::fft;
using namespace gr::kernel::math;

namespace gr {
namespace analog {

wfm_rcv_pll_cpu::wfm_rcv_pll_cpu(block_args args) : INHERITED_CONSTRUCTORS
{

    auto demod_rate = args.quad_rate;
    auto deemph_tau = args.deemph_tau;

    auto stereo_carrier_filter_coeffs = firdes::band_pass(
        -2.0, demod_rate, 37600, 38400, 400, window::window_t::HAMMING, 6.76);
    auto pilot_carrier_filter_coeffs = firdes::complex_band_pass(
        1.0, demod_rate, 18980, 19020, 1500, window::window_t::HAMMING, 6.76);
    float deviation = 75000;
    auto audio_filter_coeffs =
        firdes::low_pass(1, demod_rate, 15000, 1500, window::window_t::HAMMING, 6.76);
    auto audio_decim = args.audio_decimation;
    float audio_rate = demod_rate / audio_decim;
    auto samp_delay = (pilot_carrier_filter_coeffs.size() - 1);

    // ##################################################
    // # Blocks
    // ##################################################
    auto pilot_carrier_bpf =
        filter::fir_filter_fcc::make({ 1, pilot_carrier_filter_coeffs });
    auto stereo_carrier_bpf =
        filter::fft_filter_fff::make({ 1, stereo_carrier_filter_coeffs, 1 });
    auto stereo_audio_lpf =
        filter::fft_filter_fff::make({ audio_decim, audio_filter_coeffs, 1 });
    auto mono_audio_lpf =
        filter::fft_filter_fff::make({ audio_decim, audio_filter_coeffs, 1 });
    auto blocks_stereo_multiply = math::multiply_ff::make({ 2 });
    auto blocks_pilot_multiply = math::multiply_cc::make({ 2 });
    auto blocks_complex_to_imag = math::complex_to_imag::make({ 1 });
    auto blocks_right_sub = math::sub_ff::make({ 2 });
    auto blocks_left_add = math::add_ff::make({ 2 });
    auto analog_quadrature_demod_cf = analog::quadrature_demod::make(
        { (float)(demod_rate / (2 * GR_M_PI * deviation)) });
    auto analog_pll_refout_cc =
        analog::pll_refout::make({ 0.001,
                                   (float)(2 * GR_M_PI * 19200 / demod_rate),
                                   (float)(2 * GR_M_PI * 18800 / demod_rate) });
    auto analog_right_fm_deemph = analog::fm_deemph::make({ audio_rate, deemph_tau });
    auto analog_left_fm_deemph = analog::fm_deemph::make({ audio_rate, deemph_tau });
    auto blocks_delay_0 = streamops::delay::make({ samp_delay, sizeof(float) });

    // ##################################################
    // # Connections
    // ##################################################
    hier_block::connect(analog_left_fm_deemph, 0, self(), 0);
    hier_block::connect(analog_right_fm_deemph, 0, self(), 1);
    hier_block::connect(analog_pll_refout_cc, 0, blocks_pilot_multiply, 1);
    hier_block::connect(analog_pll_refout_cc, 0, blocks_pilot_multiply, 0);
    hier_block::connect(analog_quadrature_demod_cf, 0, blocks_delay_0, 0);
    hier_block::connect(blocks_delay_0, 0, blocks_stereo_multiply, 0);
    hier_block::connect(blocks_delay_0, 0, mono_audio_lpf, 0);
    hier_block::connect(analog_quadrature_demod_cf, 0, pilot_carrier_bpf, 0);
    hier_block::connect(blocks_left_add, 0, analog_left_fm_deemph, 0);
    hier_block::connect(blocks_right_sub, 0, analog_right_fm_deemph, 0);
    hier_block::connect(blocks_complex_to_imag, 0, stereo_carrier_bpf, 0);
    hier_block::connect(blocks_pilot_multiply, 0, blocks_complex_to_imag, 0);
    hier_block::connect(blocks_stereo_multiply, 0, stereo_audio_lpf, 0); // L - R path
    hier_block::connect(mono_audio_lpf, 0, blocks_left_add, 1);
    hier_block::connect(mono_audio_lpf, 0, blocks_right_sub, 0);
    hier_block::connect(stereo_audio_lpf, 0, blocks_left_add, 0);
    hier_block::connect(stereo_audio_lpf, 0, blocks_right_sub, 1);
    hier_block::connect(stereo_carrier_bpf, 0, blocks_stereo_multiply, 1);
    hier_block::connect(pilot_carrier_bpf, 0, analog_pll_refout_cc, 0);
    hier_block::connect(self(), 0, analog_quadrature_demod_cf, 0);
}

} // namespace analog
} // namespace gr