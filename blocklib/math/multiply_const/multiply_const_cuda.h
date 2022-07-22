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

#include <gnuradio/math/multiply_const.h>

#include <cusp/multiply_const.cuh>

namespace gr {
namespace math {

template <class T>
class multiply_const_cuda : public multiply_const<T>
{
public:
    multiply_const_cuda(const typename multiply_const<T>::block_args& args);

    work_return_code_t work(work_io&) override;

protected:
    T d_k;
    size_t d_vlen;

    cudaStream_t d_stream;
    std::shared_ptr<cusp::multiply_const<T>> p_kernel;
};


} // namespace math
} // namespace gr
