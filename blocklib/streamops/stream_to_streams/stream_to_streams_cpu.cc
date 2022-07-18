/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "stream_to_streams_cpu.h"
#include "stream_to_streams_cpu_gen.h"
#include <volk/volk.h>

namespace gr {
namespace streamops {

stream_to_streams_cpu::stream_to_streams_cpu(const block_args& args)
    : INHERITED_CONSTRUCTORS
{
}

work_return_code_t stream_to_streams_cpu::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<uint8_t>();

    uint8_t* in_ptr = const_cast<uint8_t*>(in);
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;
    size_t nstreams = wio.outputs().size();

    auto total_items = std::min(ninput_items / nstreams, (size_t)noutput_items);
    auto itemsize = wio.outputs()[0].buf().item_size();

    for (size_t i = 0; i < total_items; i++) {
        for (size_t j = 0; j < nstreams; j++) {
            memcpy(wio.outputs()[j].items<uint8_t>() + i * itemsize, in_ptr, itemsize);
            in_ptr += itemsize;
        }
    }

    wio.produce_each(total_items);
    wio.consume_each(total_items * nstreams);
    return work_return_code_t::WORK_OK;
}


} /* namespace streamops */
} /* namespace gr */
