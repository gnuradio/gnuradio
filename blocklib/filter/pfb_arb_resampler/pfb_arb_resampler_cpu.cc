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

template <>
std::vector<gr_complex>
pfb_arb_resampler_cpu<gr_complex, gr_complex, gr_complex>::create_taps(float rate,
                                                                       size_t flt_size,
                                                                       float atten)
{
    auto ftaps = create_taps_float(rate, flt_size, atten);
    std::vector<gr_complex> ctaps;
    for (auto& f : ftaps) {
        ctaps.push_back(gr_complex(f, 0.0));
    }
    return ctaps;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::create_taps(float rate,
                                                                          size_t flt_size,
                                                                          float atten)
{
    return create_taps_float(rate, flt_size, atten);
}


template <class IN_T, class OUT_T, class TAP_T>
pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::pfb_arb_resampler_cpu(
    const typename pfb_arb_resampler<IN_T, OUT_T, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(IN_T, OUT_T, TAP_T)
{

    std::vector<TAP_T> taps = args.taps;
    if (taps.empty()) {
        taps = create_taps(args.rate, args.filter_size, 100.0);
    }

    d_resamp = std::make_unique<kernel::filter::pfb_arb_resampler<IN_T, OUT_T, TAP_T>>(
        args.rate, taps, args.filter_size);
    this->declare_noconsume(d_resamp->taps_per_filter() - 1);

    this->set_relative_rate(args.rate);
    if (args.rate >= 1.0f) {
        size_t output_multiple = std::max<size_t>(args.rate, args.filter_size);
        this->set_output_multiple(output_multiple);
    }
}


template <class IN_T, class OUT_T, class TAP_T>
work_return_t pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::enforce_constraints(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.outputs()[0].n_items;

    // Constrain inputs based on outputs
    size_t nin = 0;
    size_t nout = (noutput_items / this->output_multiple()) * this->output_multiple();

    if (nout / this->relative_rate() < 1) {
        nin = this->relative_rate() + this->noconsume();
    }
    else {
        nin = nout / this->relative_rate() + this->noconsume();
    }

    if (ninput_items < nin) {
        nin = ninput_items;
        // Then we have to constrain from input to output
        nout = (((ninput_items - this->noconsume()) * this->relative_rate()) /
                         this->output_multiple()) *
                        this->output_multiple();
    }

    if (nout < this->output_multiple()) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }


    wio.inputs()[0].n_items = nin - this->noconsume();
    wio.outputs()[0].n_items = nout;

    return work_return_t::OK;
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t pfb_arb_resampler_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{

    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    auto nitems = wio.inputs()[0].n_items;

    int nitems_read;
    int processed = d_resamp->filter(out, in, nitems, nitems_read);

    wio.consume_each(nitems_read);
    wio.produce_each(processed);
    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */
