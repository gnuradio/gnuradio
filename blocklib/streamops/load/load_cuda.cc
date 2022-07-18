/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "load_cuda.h"
#include "load_cuda_gen.h"

#include <gnuradio/helper_cuda.h>

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

#include "load_cuda.cuh"

namespace gr {
namespace streamops {

load_cuda::load_cuda(block_args args)
    : INHERITED_CONSTRUCTORS, d_load(args.iterations), d_use_cb(args.use_cb)

{
    load_cu::get_block_and_grid(&d_min_grid_size, &d_block_size);
    d_logger->info("minGrid: {}, blockSize: {}", d_min_grid_size, d_block_size);
    cudaStreamCreate(&d_stream);

    if (!d_use_cb) {
        checkCudaErrors(cudaMalloc((void**)&d_dev_in, d_max_buffer_size));
        checkCudaErrors(cudaMalloc((void**)&d_dev_out, d_max_buffer_size));
    }
}

work_return_code_t load_cuda::work(work_io& wio)

{
    auto in = wio.inputs()[0].items<uint8_t>();
    auto out = wio.outputs()[0].items<uint8_t>();
    auto noutput_items = wio.outputs()[0].n_items;
    auto itemsize = wio.outputs()[0].buf().item_size();
    int gridSize = (noutput_items * itemsize + d_block_size - 1) / d_block_size;
    if (d_use_cb) {
        load_cu::exec_kernel(
            in, out, gridSize, d_block_size, noutput_items, d_load, d_stream);
        checkCudaErrors(cudaPeekAtLastError());
        cudaStreamSynchronize(d_stream);
    }
    else {
        checkCudaErrors(cudaMemcpyAsync(
            d_dev_in, in, noutput_items * itemsize, cudaMemcpyHostToDevice, d_stream));

        load_cu::exec_kernel(
            d_dev_in, d_dev_out, gridSize, d_block_size, noutput_items, d_load, d_stream);
        checkCudaErrors(cudaPeekAtLastError());

        cudaMemcpyAsync(
            out, d_dev_out, noutput_items * itemsize, cudaMemcpyDeviceToHost, d_stream);
    }

    // Tell runtime system how many output items we produced.
    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}
} // namespace streamops
} // namespace gr
