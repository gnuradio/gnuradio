/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "freq_xlating_fir_filter_cpu.h"
#include "freq_xlating_fir_filter_cpu_gen.h"

#include <gnuradio/kernel/math/math.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
freq_xlating_fir_filter_cpu<IN_T, OUT_T, TAP_T>::freq_xlating_fir_filter_cpu(const typename freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(IN_T, OUT_T, TAP_T),
      d_proto_taps(args.taps),
      d_composite_fir({}),
      d_prev_center_freq(args.center_freq)
{
    this->set_relative_rate(1.0 / args.decimation);
    d_history = this->d_proto_taps.size();
    this->build_composite_fir();
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_cpu<IN_T, OUT_T, TAP_T>::build_composite_fir()
{
    auto center_freq = pmtf::get_as<double>(*this->param_center_freq);
    auto sampling_freq = pmtf::get_as<double>(*this->param_sampling_freq);
    auto decimation = pmtf::get_as<size_t>(*this->param_decimation);

    std::vector<gr_complex> ctaps(d_proto_taps.size());

    // In order to avoid phase jumps during a retune, adjust the phase
    // of the rotator. Phase delay of a symmetric, odd length FIR is (N-1)/2.
    // Scale phase delay by delta omega to get the difference in phase response
    // caused by retuning. Subtract from the current rotator phase.

    gr_complex phase = d_r.phase();
    phase /= std::abs(phase);
    float delta_freq = center_freq - d_prev_center_freq;
    float delta_omega = 2.0 * GR_M_PI * delta_freq / sampling_freq;
    float delta_phase = -delta_omega * (d_proto_taps.size() - 1) / 2.0;
    phase *= exp(gr_complex(0, delta_phase));
    d_r.set_phase(phase);

    // The basic principle of this block is to perform:
    //    x(t) -> (mult by -fwT0) -> LPF -> decim -> y(t)
    // We switch things up here to:
    //    x(t) -> BPF -> decim -> (mult by fwT0*decim) -> y(t)
    // The BPF is the baseband filter (LPF) moved up to the
    // center frequency fwT0. We then apply a derotator
    // with -fwT0 to downshift the signal to baseband.



    float fwT0 = 2 * GR_M_PI * center_freq / sampling_freq;
    for (unsigned int i = 0; i < d_proto_taps.size(); i++) {
        ctaps[i] = d_proto_taps[i] * exp(gr_complex(0, i * fwT0));
    }

    d_composite_fir.set_taps(ctaps);
    d_r.set_phase_incr(exp(gr_complex(0, -fwT0 * decimation)));
    d_prev_center_freq = center_freq;
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_cpu<IN_T, OUT_T, TAP_T>::on_parameter_change(param_action_sptr action)
{
    // This will set the underlying PMT
    block::on_parameter_change(action);

    // Do more updating for certain parameters
    if (action->id() == freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::id_taps) {
        d_proto_taps = pmtf::get_as<std::vector<TAP_T>>(*this->param_taps);
        d_updated = true;
    }
    else if (action->id() == freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::id_center_freq) {
        d_updated = true;
    }
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t freq_xlating_fir_filter_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{

    // Do forecasting
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto decim = pmtf::get_as<size_t>(*this->param_decimation);
    // rebuild composite FIR if the center freq has changed
    if (d_updated) {
        d_hist_change = d_history - d_proto_taps.size();
        d_history = d_proto_taps.size();
        d_updated = false;
        d_hist_updated = true;
        this->build_composite_fir();
    }

    auto min_ninput = std::min(noutput * decim + d_history - 1, ninput - (d_history - 1));
    // auto noutput_items = std::min( (min_ninput + decim - 1) / decim, noutput);
    auto noutput_items = std::min(min_ninput / decim, noutput);

    if (noutput_items <= 0) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    unsigned j = 0;
    for (size_t i = 0; i < noutput_items; i++) {
        out[i] = d_composite_fir.filter(&in[j]);
        j += decim;
    }

    // re-use of the same buffer as the input and output is safe for many volk functions
    // and faster than creating local temporary memory in the work function and doing an
    // extra copy. So out is used below as both the in and out params to the rotate
    // function.
    d_r.rotateN(out, out, noutput_items);

    wio.consume_each(noutput_items);
    wio.produce_each(noutput_items);

    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */
