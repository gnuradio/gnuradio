/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "null_source_cpu.h"
#include "null_source_cpu_gen.h"

namespace gr {
namespace blocks {

null_source_cpu::null_source_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_code_t null_source_cpu::work(work_io& wio)
{
    auto itemsize = wio.outputs()[0].buf().item_size();
    for (auto& out : wio.outputs()) {
        auto noutput_items = out.n_items;
        memset(out.raw_items(), 0, noutput_items * itemsize);
        out.n_produced = noutput_items;
    }

    return work_return_code_t::WORK_OK;
}


} // namespace blocks
} // namespace gr