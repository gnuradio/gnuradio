/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "nop_head_cpu.h"
#include "nop_head_cpu_gen.h"

namespace gr {
namespace streamops {

nop_head_cpu::nop_head_cpu(const block_args& args)
    : INHERITED_CONSTRUCTORS, d_nitems(args.nitems)
{
}

work_return_code_t nop_head_cpu::work(work_io& wio)

{

    if (d_ncopied_items >= d_nitems) {
        wio.outputs()[0].n_produced = 0;
        return work_return_code_t::WORK_DONE; // Done!
    }

    unsigned n = std::min(d_nitems - d_ncopied_items, (uint64_t)wio.outputs()[0].n_items);

    if (n == 0) {
        wio.outputs()[0].n_produced = 0;
        return work_return_code_t::WORK_OK;
    }

    // Do Nothing

    d_ncopied_items += n;
    wio.outputs()[0].n_produced = n;

    return work_return_code_t::WORK_OK;
}

} /* namespace streamops */
} /* namespace gr */
