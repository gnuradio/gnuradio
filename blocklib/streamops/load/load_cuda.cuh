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
#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace streamops {
namespace load_cu {

void exec_kernel(
    const uint8_t* in, uint8_t* out, int grid_size, int block_size, int N, size_t load, cudaStream_t stream);

void get_block_and_grid(int* minGrid, int* minBlock);

} // namespace multiply_const
} // namespace streamops
} // namespace gr