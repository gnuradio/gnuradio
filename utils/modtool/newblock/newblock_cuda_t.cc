/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "newblock_cuda.cuh"
#include "newblock_cuda.h"
#include "newblock_cuda_gen.h"

namespace gr {
namespace newmod {

template <class T>
newblock_cuda<T>::newblock_cuda(const typename newblock<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
    newblock_cu::get_block_and_grid<T>(&d_min_grid_size, &d_block_size);
    cudaStreamCreate(&d_stream);
}

template <>
newblock_cuda<gr_complex>::newblock_cuda(
    const typename newblock<gr_complex>::block_args& args)
    : INHERITED_CONSTRUCTORS(gr_complex)
{
    newblock_cu::get_block_and_grid<cuFloatComplex>(&d_min_grid_size, &d_block_size);
    cudaStreamCreate(&d_stream);
}


template <class T>
work_return_code_t newblock_cuda<T>::work(work_io& wio)
{
    // Do block specific code here
    return work_return_code_t::WORK_OK;
}

template <>
work_return_code_t newblock_cuda<gr_complex>::work(work_io& wio)
{
    // Do block specific code here
    return work_return_code_t::WORK_OK;
}

} /* namespace newmod */
} /* namespace gr */
