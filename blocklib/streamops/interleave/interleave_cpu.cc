/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "interleave_cpu.h"
#include "interleave_cpu_gen.h"

namespace gr {
namespace streamops {

interleave_cpu::interleave_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      d_ninputs(args.nstreams),
      d_blocksize(args.blocksize),
      d_itemsize(args.itemsize)

{
    set_relative_rate(d_ninputs);
    set_output_multiple(d_blocksize * d_ninputs);
}

work_return_code_t interleave_cpu::work(work_io& wio)

{

    // Since itemsize can be set after construction
    if (d_itemsize == 0) {
        d_itemsize = wio.inputs()[0].buf().item_size();
        return work_return_code_t::WORK_OK;
    }

    // Forecasting
    auto ninput_items = wio.min_ninput_items();
    auto noutput_items = wio.outputs()[0].n_items;
    auto noutput_blocks =
        std::min(ninput_items / d_blocksize, noutput_items / (d_blocksize * d_ninputs));

    if (noutput_blocks < 1) {
        return work_return_code_t::WORK_INSUFFICIENT_OUTPUT_ITEMS;
    }

    auto out = wio.outputs()[0].items<uint8_t>();

    for (unsigned int i = 0; i < noutput_blocks; i++) {
        for (auto& in : wio.inputs()) {
            memcpy(out,
                   in.items<uint8_t>() + d_itemsize * d_blocksize * i,
                   d_itemsize * d_blocksize);
            out += d_itemsize * d_blocksize;
        }
    }
    wio.consume_each(noutput_blocks * d_blocksize);
    wio.produce_each(noutput_blocks * d_blocksize * d_ninputs);
    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr