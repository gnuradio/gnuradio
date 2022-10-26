/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fft_filter_cpu.h"
#include "fft_filter_cpu_gen.h"

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
fft_filter_cpu<IN_T, OUT_T, TAP_T>::fft_filter_cpu(
    const typename fft_filter<IN_T, OUT_T, TAP_T>::block_args& args)
    : INHERITED_CONSTRUCTORS(IN_T, OUT_T, TAP_T),
      d_filter(args.decimation, args.taps, args.nthreads)
{
    d_nsamples = d_filter.set_taps(args.taps);
    this->set_output_multiple(d_nsamples);
    // this->declare_noconsume(d_filter.ntaps() - 1);
    this->set_relative_rate(1.0 / args.decimation);
}

template <class IN_T, class OUT_T, class TAP_T>
void fft_filter_cpu<IN_T, OUT_T, TAP_T>::on_parameter_change(param_action_sptr action)
{
    // This will set the underlying PMT
    block::on_parameter_change(action);

    // Do more updating for certain parameters
    if (action->id() == fft_filter<IN_T, OUT_T, TAP_T>::id_taps) {
        auto taps = pmtf::get_as<std::vector<TAP_T>>(*this->param_taps);
        d_nsamples = d_filter.set_taps(taps);
        this->set_output_multiple(d_nsamples);
        // this->declare_noconsume(d_filter.ntaps() - 1);
    }
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t fft_filter_cpu<IN_T, OUT_T, TAP_T>::enforce_constraints(work_io& wio)
{
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto decim = pmtf::get_as<size_t>(*this->param_decimation);

        // if (!d_startup_complete) {
        //     d_startup_nsamps = this->noconsume();
        //     // round up the input vector size to the next output multiple
        //     auto input_mult = decim * this->output_multiple();
        //     size_t vec_size =
        //         ((d_startup_nsamps * 2 - 1 + input_mult - 1) / input_mult) * input_mult;
        //     d_startup_vec.resize(vec_size);
        //     d_startup_complete = true;
        // }

    auto min_ninput =
        std::min(noutput * decim + this->noconsume(),
                 (ninput > this->noconsume()) ? ninput - this->noconsume() : 0);
    auto mult = this->output_multiple();

    auto noutput_items = std::min(min_ninput / decim, noutput);
    noutput_items = (noutput_items / mult) * mult; // ensure output_multiple is preserved

    if (noutput_items <= 0) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    wio.outputs()[0].n_items = noutput_items;
    wio.inputs()[0].n_items = noutput_items * decim;

    return work_return_t::OK;
}

template <class IN_T, class OUT_T, class TAP_T>
work_return_t fft_filter_cpu<IN_T, OUT_T, TAP_T>::work(work_io& wio)
{
    size_t noutput_items = wio.outputs()[0].n_items;
    size_t ninput_items = wio.inputs()[0].n_items;

    auto in = wio.inputs()[0].items<IN_T>();
    auto out = wio.outputs()[0].items<OUT_T>();

    // Handle the startup transients
    // If we have not pushed ntaps-1 through the filter, then do so
    if (false) { //(d_startup_nsamps) {
        // auto decim = pmtf::get_as<size_t>(*this->param_decimation);
        // size_t input_mult = decim * this->output_multiple();
        // // need enough samples to get output_multiple() through the filter obj
        // // round d_startup_nsamps up to the next output_multiple*decim
        // size_t nfilt = ((d_startup_nsamps + input_mult - 1) / input_mult) * input_mult;
        // nfilt = std::min(nfilt, ninput_items);
        // size_t npad = std::min(nfilt, d_startup_nsamps);

        // std::copy(
        //     in, in + nfilt - npad, d_startup_vec.data() + npad);
        // d_filter.filter(noutput_items, d_startup_vec.data(), out);

        // // how many of the input items that we filtered were startup padding
        // ninput_items -= npad;
        // d_startup_nsamps -= npad;
    }
    else {
        d_filter.filter(noutput_items, in, out);
    }


    wio.consume_each(ninput_items);
    wio.produce_each(noutput_items);

    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */
