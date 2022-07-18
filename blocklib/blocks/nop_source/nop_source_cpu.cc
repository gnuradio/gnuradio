/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "nop_source_cpu.h"
#include "nop_source_cpu_gen.h"

namespace gr {
namespace blocks {

nop_source_cpu::nop_source_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_code_t nop_source_cpu::work(work_io& wio)
{
    for (auto& out : wio.outputs()) {
        auto noutput_items = out.n_items;
        out.n_produced = noutput_items;
    }

    return work_return_code_t::WORK_OK;
}


} // namespace blocks
} // namespace gr