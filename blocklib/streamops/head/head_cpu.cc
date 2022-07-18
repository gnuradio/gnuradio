/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "head_cpu.h"
#include "head_cpu_gen.h"

namespace gr {
namespace streamops {

head_cpu::head_cpu(const block_args& args) : INHERITED_CONSTRUCTORS, d_nitems(args.nitems)
{
}

work_return_code_t head_cpu::work(work_io& wio)
{
    auto iptr = wio.inputs()[0].items<uint8_t>();
    auto optr = wio.outputs()[0].items<uint8_t>();

    if (d_ncopied_items >= d_nitems) {
        wio.outputs()[0].n_produced = 0;
        return work_return_code_t::WORK_DONE; // Done!
    }

    unsigned n = std::min(d_nitems - d_ncopied_items, (uint64_t)wio.outputs()[0].n_items);

    if (n == 0) {
        wio.outputs()[0].n_produced = 0;
        return work_return_code_t::WORK_OK;
    }

    memcpy(optr, iptr, n * wio.inputs()[0].buf().item_size());

    d_ncopied_items += n;
    wio.outputs()[0].n_produced = n;

    if (d_ncopied_items >= d_nitems) {
        return work_return_code_t::WORK_DONE; // Done!
    }
    return work_return_code_t::WORK_OK;
}

} /* namespace streamops */
} /* namespace gr */
