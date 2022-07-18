/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "newblock_cuda.h"
#include "newblock_cuda_gen.h"

#include <gnuradio/helper_cuda.h>

#include "newblock_cuda.cuh"
namespace gr {
namespace newmod {

newblock_cuda::newblock_cuda(block_args args) : INHERITED_CONSTRUCTORS

{
    newblock_cu::get_block_and_grid(&d_min_grid_size, &d_block_size);
    d_logger->debug("minGrid: {}, blockSize: {}", d_min_grid_size, d_block_size);

    cudaStreamCreate(&d_stream);
}

work_return_code_t newblock_cuda::work(work_io& wio)
{
    // Do <+signal processing+>
    // Block specific code goes here
    cudaStreamSynchronize(d_stream);

    // produce_each(n, work_output);
    return work_return_code_t::WORK_OK;
}
} // namespace newmod
} // namespace gr
