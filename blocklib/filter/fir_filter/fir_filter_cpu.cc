/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fir_filter_cpu.h"
#include "fir_filter_cpu_gen.h"

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
fir_filter_cpu<IN_T, OUT_T, TAP_T>::fir_filter_cpu(
    const typename fir_filter<IN_T, OUT_T, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(IN_T, OUT_T, TAP_T), d_fir(args.taps)
{
    this->set_relative_rate(1.0 / args.decimation);

    // const int alignment_multiple = volk_get_alignment() / sizeof(float);
    // this->set_alignment(std::max(1, alignment_multiple));
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter_cpu<IN_T, OUT_T, TAP_T>::on_parameter_change(param_action_sptr action)
{
    // This will set the underlying PMT
    block::on_parameter_change(action);

    // Do more updating for certain parameters
    if (action->id() == fir_filter<IN_T, OUT_T, TAP_T>::id_taps) {
        auto taps = pmtf::get_as<std::vector<TAP_T>>(*this->param_taps);
        d_fir.set_taps(taps);
        d_updated = true;
    }
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_code_t fir_filter_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{
    // Do forecasting
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto decim = pmtf::get_as<size_t>(*this->param_decimation);

    if (d_updated) {
        d_hist_change = d_history - d_fir.ntaps();
        d_history = d_fir.ntaps();
        d_updated = false;
        d_hist_updated = true;
    }

    auto min_ninput = std::min(noutput * decim + d_history - 1, ninput - (d_history - 1));
    // auto noutput_items = std::min( (min_ninput + decim - 1) / decim, noutput);
    auto noutput_items = std::min(min_ninput / decim, noutput);

    if (noutput_items <= 0) {
        return work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS;
    }


    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    if (decim == 1) {
        d_fir.filterN(out, in, noutput_items);
    }
    else {
        d_fir.filterNdec(out, in, noutput_items, decim);
    }

    // wio.consume_each(noutput_items * decim + d_hist_change);
    wio.consume_each(noutput_items * decim);
    wio.produce_each(noutput_items);

    if (d_hist_updated) {
        d_hist_change = 0;
        d_hist_updated = false;
    }
    return work_return_code_t::WORK_OK;
}

} /* namespace filter */
} /* namespace gr */
