/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

// The block cuda file is just a wrapper for the kernels that will be launched in the work
// function
namespace gr {
namespace streamops {
namespace load_cu {
    __global__ void load_kernel(const uint8_t* in, uint8_t* out, int N, size_t load = 1)
    {
        int i = blockIdx.x * blockDim.x + threadIdx.x;
        if (i < N) {
            for (int x = 0; x < load; x++) {
                out[i] = in[i];
            }
        }
    }
    
    void exec_kernel(const uint8_t* in,
                     uint8_t* out,
                     int grid_size,
                     int block_size,
                     int N,
                     size_t load,
                     cudaStream_t stream)
    {
        load_kernel<<<grid_size, block_size, 0, stream>>>(in, out, N, load);
    }
    

void get_block_and_grid(int* minGrid, int* minBlock)
{
    cudaOccupancyMaxPotentialBlockSize(minGrid, minBlock, load_kernel, 0, 0);
}

} // namespace load_cu
} // namespace streamops
} // namespace gr