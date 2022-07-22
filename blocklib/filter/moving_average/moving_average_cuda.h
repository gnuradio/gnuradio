/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/filter/moving_average.h>

#include <vector>

#include <cusp/convolve.cuh>

namespace gr {
namespace filter {

template <class T>
class moving_average_cuda : public moving_average<T>
{
public:
    moving_average_cuda(const typename moving_average<T>::block_args& args);

    work_return_code_t work(work_io&) override;

    int group_delay();

protected:
    size_t d_length;
    T d_scale;
    size_t d_max_iter;
    size_t d_vlen;

    size_t d_new_length;
    T d_new_scale;
    bool d_updated = false;

    std::shared_ptr<cusp::convolve<T, T>> p_kernel_full;
    std::shared_ptr<cusp::convolve<T, T>> p_kernel_valid;
};


} // namespace filter
} // namespace gr
