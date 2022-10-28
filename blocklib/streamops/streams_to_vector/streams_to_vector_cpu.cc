/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "streams_to_vector_cpu.h"
#include "streams_to_vector_cpu_gen.h"

namespace gr {
namespace streamops {

streams_to_vector_cpu::streams_to_vector_cpu(block_args args)
    : INHERITED_CONSTRUCTORS, d_vlen(args.nstreams)
{
}

work_return_t streams_to_vector_cpu::work(work_io& wio)
{
    auto itemsize = wio.inputs()[0].buf().item_size();
    auto noutput_items = wio.outputs()[0].n_items;
    auto out = wio.outputs()[0].items<uint8_t>();

    for (size_t i = 0; i < noutput_items; i++) {
        for (size_t j = 0; j < d_vlen; j++) {
            auto in = wio.inputs()[j].items<uint8_t>() + i * itemsize;
            std::copy(in, in + itemsize, out);
            out += itemsize;
        }
    }

    wio.produce_each(noutput_items);
    return work_return_t::OK;
}


} // namespace streamops
} // namespace gr