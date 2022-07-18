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

// The block cuda file is just a wrapper for the kernels that will be launched in the work
// function
namespace gr {
namespace newmod {
namespace newblock_cu {
__global__ void newblock_kernel(const uint8_t* in, uint8_t* out, int batch_size)
{
    // block specific code goes here
}

void exec_newblock(
    const uint8_t* in, uint8_t* out, int grid_size, int block_size, cudaStream_t stream)
{
    int batch_size = block_size * grid_size;
    newblock_kernel<<<grid_size, block_size, 0, stream>>>(in, out, batch_size);
}

void get_block_and_grid(int* minGrid, int* minBlock)
{
    // https://developer.nvidia.com/blog/cuda-pro-tip-occupancy-api-simplifies-launch-configuration/
    cudaOccupancyMaxPotentialBlockSize(minGrid, minBlock, newblock_kernel, 0, 0);
}
} // namespace newblock_cu
} // namespace newmod
} // namespace gr