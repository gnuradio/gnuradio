/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/newmod/newblock.h>

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace newmod {

class newblock_cuda : public newblock
{
public:
    newblock_cuda(block_args args);
    work_return_code_t work(work_io&) override;

private:
    int d_block_size;
    int d_min_grid_size;
    cudaStream_t d_stream;
};

} // namespace newmod
} // namespace gr
