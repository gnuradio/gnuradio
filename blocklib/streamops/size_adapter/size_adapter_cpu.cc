/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "size_adapter_cpu.h"
#include "size_adapter_cpu_gen.h"

namespace gr {
namespace streamops {

size_adapter_cpu::size_adapter_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_t size_adapter_cpu::work(work_io& wio)
{
    if (!_itemsize_in) {
        _itemsize_in = wio.inputs()[0].buf().item_size();
    }

    if (!_itemsize_out) {
        _itemsize_out = wio.outputs()[0].buf().item_size();
    }

    auto nin = wio.inputs()[0].n_items;
    auto nout = wio.outputs()[0].n_items;

    auto in = wio.inputs()[0].items<uint8_t>();
    auto out = wio.outputs()[0].items<uint8_t>();

    auto min_bytes = std::min(nout * _itemsize_out, nin * _itemsize_in);
    auto nproduce = min_bytes / _itemsize_out;
    auto nconsume = min_bytes / _itemsize_in;

    if (nproduce == 0) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    if (nconsume == 0) {
        return work_return_t::INSUFFICIENT_OUTPUT_ITEMS;
    }

    // This block might be an interpolator or decimator
    memcpy(out, in, min_bytes);


    wio.consume_each(nconsume);
    wio.produce_each(nproduce);


    return work_return_t::OK;
}


} // namespace streamops
} // namespace gr