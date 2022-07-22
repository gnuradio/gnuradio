/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "nop_cpu.h"
#include "nop_cpu_gen.h"

namespace gr {
namespace streamops {

nop_cpu::nop_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_code_t nop_cpu::work(work_io& wio)

{
    wio.produce_each(wio.outputs()[0].n_items);
    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr