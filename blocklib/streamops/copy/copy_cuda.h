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

#include <gnuradio/streamops/copy.h>

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace streamops {

class copy_cuda : public copy
{
public:
    copy_cuda(block_args args);
    work_return_code_t work(work_io&) override;

protected:
    int d_block_size;
    int d_min_grid_size;
    cudaStream_t d_stream;
};

} // namespace streamops
} // namespace gr