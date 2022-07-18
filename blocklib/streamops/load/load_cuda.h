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

#include <gnuradio/streamops/load.h>

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace streamops {

class load_cuda : public load
{
public:
    load_cuda(block_args args);
    work_return_code_t work(work_io&) override;

protected:
    size_t d_load;
    bool d_use_cb;

    int d_block_size;
    int d_min_grid_size;
    cudaStream_t d_stream;

    uint8_t* d_dev_in;
    uint8_t* d_dev_out;

    size_t d_max_buffer_size = 65536 * 8;
};

} // namespace streamops
} // namespace gr