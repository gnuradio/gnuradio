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
    this->declare_noconsume(d_fir.ntaps() - 1);
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
        this->declare_noconsume(d_fir.ntaps() - 1);
    }
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t fir_filter_cpu<IN_T, OUT_T, TAP_T>::enforce_constraints(work_io& wio)
{
    // Do forecasting
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto decim = pmtf::get_as<size_t>(*this->param_decimation);

    auto min_ninput =
        std::min(noutput * decim + this->noconsume(), ninput - this->noconsume());
    auto noutput_items = std::min(min_ninput / decim, noutput);

    if (noutput_items <= 0) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    wio.outputs()[0].n_items = noutput_items;
    wio.inputs()[0].n_items = noutput_items * decim;

    return work_return_t::OK;
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t fir_filter_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    auto decim = pmtf::get_as<size_t>(*this->param_decimation);

    if (decim == 1) {
        d_fir.filterN(out, in, noutput);
    }
    else {
        d_fir.filterNdec(out, in, noutput, decim);
    }

    wio.consume_each(ninput);
    wio.produce_each(noutput);

    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */
