/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2013,2017,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "moving_average_cpu.h"
#include "moving_average_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace filter {

template <class T>
moving_average_cpu<T>::moving_average_cpu(
    const typename moving_average<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      d_length(args.length),
      d_scale(args.scale),
      d_max_iter(args.max_iter),
      d_vlen(args.vlen),
      d_new_length(args.length),
      d_new_scale(args.scale)
{
    d_sum = std::vector<T>(d_vlen);
    d_history = std::vector<T>(d_vlen * (d_length - 1));
}

template <class T>
work_return_code_t moving_average_cpu<T>::work(work_io& wio)
{
    if (wio.inputs()[0].n_items < d_length) {
        wio.outputs()[0].n_produced = 0;
        wio.inputs()[0].n_consumed = 0;
        return work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS;
    }

    if (d_updated) {
        d_length = d_new_length;
        d_scale = d_new_scale;
        d_updated = false;
        wio.outputs()[0].n_produced = 0;
        wio.inputs()[0].n_consumed = 0;
        return work_return_code_t::WORK_OK;
    }

    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();

    size_t noutput_items =
        std::min((wio.inputs()[0].n_items - d_length), wio.outputs()[0].n_items);

    auto num_iter = (noutput_items > d_max_iter) ? d_max_iter : noutput_items;
    auto tr = wio.inputs()[0].buf().total_read();

    if (tr == 0) { // for the first no history case
        for (size_t i = 0; i < num_iter; i++) {
            for (size_t elem = 0; elem < d_vlen; elem++) {
                d_sum[elem] += in[i * d_vlen + elem];
                out[i * d_vlen + elem] = d_sum[elem] * d_scale;
                if (i >= (d_length - 1)) {
                    d_sum[elem] -= in[(i - (d_length - 1)) * d_vlen + elem];
                }
            }
        }
    }
    else {
        for (size_t i = 0; i < num_iter; i++) {
            for (size_t elem = 0; elem < d_vlen; elem++) {

                d_sum[elem] += in[(i + d_length - 1) * d_vlen + elem];
                out[i * d_vlen + elem] = d_sum[elem] * d_scale;
                d_sum[elem] -= in[i * d_vlen + elem];
            }
        }
    }

    // don't consume the last d_length-1 samples
    wio.outputs()[0].n_produced = num_iter;
    wio.inputs()[0].n_consumed = tr == 0 ? num_iter - (d_length - 1) : num_iter;
    return work_return_code_t::WORK_OK;
} // namespace filter

} // namespace filter
} /* namespace gr */
