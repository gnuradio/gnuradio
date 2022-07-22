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

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>

namespace gr {
namespace newmod {
namespace newblock_cu {

void exec_newblock(
    const uint8_t* in, uint8_t* out, int grid_size, int block_size, cudaStream_t stream);

void get_block_and_grid(int* minGrid, int* minBlock);

} // namespace newblock_cu
} // namespace newmod
} // namespace gr