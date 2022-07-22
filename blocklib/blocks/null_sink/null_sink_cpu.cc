/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "null_sink_cpu.h"
#include "null_sink_cpu_gen.h"

namespace gr {
namespace blocks {

null_sink_cpu::null_sink_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_code_t null_sink_cpu::work(work_io& wio)
{
    return work_return_code_t::WORK_OK;
}


} // namespace blocks
} // namespace gr