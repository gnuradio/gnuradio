/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/filter/moving_average.h>
#include <volk/volk_alloc.hh>
#include <algorithm>
#include <vector>

namespace gr {
namespace filter {

template <class T>
class moving_average_cpu : public moving_average<T>
{
public:
    moving_average_cpu(const typename moving_average<T>::block_args& args);

    work_return_t work(work_io& wio) override;

private:
    work_return_t enforce_constraints(work_io& wio) override;

    volk::vector<T> d_scales;
    size_t d_max_iter;
    const size_t d_vlen;
    volk::vector<T> d_sum;

    bool d_init_padding = false;
    size_t d_padding_samps = 0;
    std::vector<T> d_padding_vec;
};


} // namespace filter
} // namespace gr
