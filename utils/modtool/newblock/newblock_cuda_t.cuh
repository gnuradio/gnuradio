/* -*- c++ -*- */
/*
 * Copyright <COPYRIGHT_YEAR> <COPYRIGHT_AUTHOR>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <cuComplex.h>
#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace newmod {
namespace newblock_cu {

template <typename T>
void exec_kernel(
    const T* in, T* out, T k, int grid_size, int block_size, cudaStream_t stream);

template <typename T>
void get_block_and_grid(int* minGrid, int* minBlock);

} // namespace newblock
} // namespace newmod
} // namespace gr