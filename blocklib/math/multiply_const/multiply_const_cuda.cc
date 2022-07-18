/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "multiply_const_cuda.h"
#include "multiply_const_cuda_gen.h"
#include <thrust/complex.h>
#include <volk/volk.h>

namespace gr {
namespace math {

template <class T>
multiply_const_cuda<T>::multiply_const_cuda(
    const typename multiply_const<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_k(args.k), d_vlen(args.vlen)
{
    p_kernel = std::make_shared<cusp::multiply_const<T>>(args.k);

    cudaStreamCreate(&d_stream);
    p_kernel->set_stream(d_stream);
}

template <>
multiply_const_cuda<gr_complex>::multiply_const_cuda(
    const typename multiply_const<gr_complex>::block_args& args)
    : INHERITED_CONSTRUCTORS(gr_complex), d_k(args.k), d_vlen(args.vlen)
{
    p_kernel = std::make_shared<cusp::multiply_const<gr_complex>>(
        (thrust::complex<float>)args.k);

    cudaStreamCreate(&d_stream);
    p_kernel->set_stream(d_stream);
}

template <class T>
work_return_code_t multiply_const_cuda<T>::work(work_io& wio)
{
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    p_kernel->launch_default_occupancy(
        { wio.inputs()[0].items<T>() }, { out }, noutput_items);
    cudaStreamSynchronize(d_stream);

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    wio.inputs()[0].n_consumed = wio.inputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}

} /* namespace math */
} /* namespace gr */
