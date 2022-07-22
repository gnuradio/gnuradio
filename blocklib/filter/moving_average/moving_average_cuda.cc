/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "moving_average_cuda.h"
#include "moving_average_cuda_gen.h"

namespace gr {
namespace filter {

template <class T>
moving_average_cuda<T>::moving_average_cuda(
    const typename moving_average<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      d_length(args.length),
      d_scale(args.scale),
      d_max_iter(args.max_iter),
      d_vlen(args.vlen),
      d_new_length(args.length),
      d_new_scale(args.scale)
{
    std::vector<T> taps(d_length);
    for (size_t i = 0; i < d_length; i++) {
        taps[i] = (float)1.0 * d_scale;
    }

    p_kernel_full =
        std::make_shared<cusp::convolve<T, T>>(taps, cusp::convolve_mode_t::FULL_TRUNC);
    p_kernel_valid =
        std::make_shared<cusp::convolve<T, T>>(taps, cusp::convolve_mode_t::VALID);
}

template <class T>
work_return_code_t moving_average_cuda<T>::work(work_io& wio)
{
    if (wio.inputs()[0].n_items < d_length) {
        wio.outputs()[0].n_produced = 0;
        wio.inputs()[0].n_consumed = 0;
        return work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS;
    }

    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();

    size_t noutput_items = std::min((wio.inputs()[0].n_items), wio.outputs()[0].n_items);

    // auto num_iter = (noutput_items > d_max_iter) ? d_max_iter : noutput_items;
    auto num_iter = noutput_items;
    auto tr = wio.inputs()[0].buf().total_read();

    if (tr == 0) {
        p_kernel_full->launch_default_occupancy({ in }, { out }, num_iter);
    }
    else {
        p_kernel_valid->launch_default_occupancy({ in }, { out }, num_iter);
    }

    // don't consume the last d_length-1 samples
    wio.outputs()[0].n_produced = tr == 0 ? num_iter : num_iter - (d_length - 1);
    wio.inputs()[0].n_consumed =
        tr == 0 ? num_iter - (d_length - 1) : num_iter - (d_length - 1);
    return work_return_code_t::WORK_OK;
} // namespace filter

} // namespace filter
} /* namespace gr */
