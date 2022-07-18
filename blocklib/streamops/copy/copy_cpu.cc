/* -*- c++ -*- */
/*
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "copy_cpu.h"
#include "copy_cpu_gen.h"
namespace gr {
namespace streamops {

copy_cpu::copy_cpu(block_args args) : INHERITED_CONSTRUCTORS {}
work_return_code_t copy_cpu::work(work_io& wio)
{
    auto iptr = wio.inputs()[0].items<uint8_t>();
    int size = wio.outputs()[0].n_items * wio.outputs()[0].buf().item_size();
    auto optr = wio.outputs()[0].items<uint8_t>();
    // std::copy(iptr, iptr + size, optr);
    memcpy(optr, iptr, size);

    wio.outputs()[0].n_produced = wio.outputs()[0].n_items;
    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr