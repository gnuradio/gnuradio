/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "stream_to_vector_cpu.h"
#include "stream_to_vector_cpu_gen.h"

namespace gr {
namespace streamops {

stream_to_vector_cpu::stream_to_vector_cpu(block_args args)
    : INHERITED_CONSTRUCTORS, d_vlen(args.vlen)
{
}

work_return_t stream_to_vector_cpu::enforce_constraints(work_io& wio)
{
    size_t ninput = wio.inputs()[0].n_items;
    size_t noutput = wio.outputs()[0].n_items;

    auto min_ninput = std::min(noutput * d_vlen, ninput);
    auto noutput_items = std::min(min_ninput / d_vlen, noutput);
    if (noutput_items <= 0) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    wio.inputs()[0].n_items = noutput_items * d_vlen;
    wio.outputs()[0].n_items = noutput_items;

    return work_return_t::OK;
}

work_return_t stream_to_vector_cpu::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<uint8_t>();
    auto out = wio.outputs()[0].items<uint8_t>();
    auto noutput_items = wio.outputs()[0].n_items;

    // use the input itemsize
    auto itemsize = wio.inputs()[0].buf().item_size();

    std::copy(in, in + itemsize * noutput_items * d_vlen, out);

    wio.produce_each(noutput_items);
    wio.consume_each(noutput_items * d_vlen);

    return work_return_t::OK;
}


} // namespace streamops
} // namespace gr