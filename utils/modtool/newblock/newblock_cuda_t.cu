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
#include <cuComplex.h>
#include <complex>

namespace gr {
namespace newmod {
namespace newblock_cu {

template <typename T>
__global__ void newblock_kernel(const T* in, T* out, T k, size_t n)
{
    // Do block specific code here
}

template <>
__global__ void newblock_kernel(const cuFloatComplex* in,
                                      cuFloatComplex* out,
                                      cuFloatComplex k,
                                      size_t n)
{
    // Do block specific code here
}

template <typename T>
void exec_kernel(
    const T* in, T* out, T k, int grid_size, int block_size, cudaStream_t stream)
{
    newblock_kernel<T>
        <<<grid_size, block_size, 0, stream>>>(in, out, k, block_size * grid_size);
}

template <typename T>
void get_block_and_grid(int* minGrid, int* minBlock)
{
    // https://developer.nvidia.com/blog/cuda-pro-tip-occupancy-api-simplifies-launch-configuration/
    cudaOccupancyMaxPotentialBlockSize(minGrid, minBlock, newblock_kernel<T>, 0, 0);
}

#define IMPLEMENT_KERNEL(T)                                                            \
    template void exec_kernel<T>(                                                      \
        const T* in, T* out, T k, int grid_size, int block_size, cudaStream_t stream); \
    template void get_block_and_grid<T>(int* minGrid, int* minBlock);


IMPLEMENT_KERNEL(int16_t)
IMPLEMENT_KERNEL(int32_t)
IMPLEMENT_KERNEL(float)
IMPLEMENT_KERNEL(cuFloatComplex)

} // namespace newblock_cu
} // namespace newmod
} // namespace gr