/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "load_cpu.h"
#include "load_cpu_gen.h"

namespace gr {
namespace streamops {

load_cpu::load_cpu(block_args args) : INHERITED_CONSTRUCTORS, d_load(args.iterations) {}

work_return_code_t load_cpu::work(work_io& wio)

{
    auto iptr = wio.inputs()[0].items<uint8_t>();
    auto optr = wio.outputs()[0].items<uint8_t>();
    int size = wio.outputs()[0].n_items * wio.outputs()[0].buf().item_size();

    // std::load(iptr, iptr + size, optr);
    for (size_t i = 0; i < d_load; i++)
        memcpy(optr, iptr, size);

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr