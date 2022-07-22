/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pfb_arb_resampler_cpu.h"
#include "pfb_arb_resampler_cpu_gen.h"

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::pfb_arb_resampler_cpu(
    const typename pfb_arb_resampler<IN_T, OUT_T, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(IN_T, OUT_T, TAP_T),
      d_resamp(args.rate, args.taps, args.filter_size)
{
    d_history = d_resamp.taps_per_filter();
    this->set_relative_rate(args.rate);
    if (args.rate >= 1.0f) {
        size_t output_multiple = std::max(args.rate, args.filter_size);
        this->set_output_multiple(output_multiple);
    }
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_code_t pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;

    // forecasting - calculate the min input to produce output_multiple output item(s)
    size_t min_ninput;
    if (this->relative_rate() > 1) {
        min_ninput =
            ceil(this->output_multiple() * this->relative_rate()) + d_history - 1;
    }
    else {
        min_ninput =
            ceil(this->output_multiple() / this->relative_rate()) + d_history - 1;
    }

    auto ninput_items = wio.inputs()[0].n_items;
    if (ninput_items < min_ninput) {
        return work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS;
    }

    size_t nitems =
        std::min<size_t>(ninput_items - d_history + 1,
                         floorf((float)noutput_items / this->relative_rate()));

    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    int nitems_read;
    int processed = d_resamp.filter(out, in, nitems, nitems_read);

    wio.consume_each(nitems_read);
    wio.produce_each(processed);
    return work_return_code_t::WORK_OK;
}

} /* namespace filter */
} /* namespace gr */
