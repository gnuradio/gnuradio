/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "delay_cpu.h"
#include "delay_cpu_gen.h"

namespace gr {
namespace streamops {

delay_cpu::delay_cpu(const block_args& args) : INHERITED_CONSTRUCTORS
{
    set_dly(args.dly);

    set_tag_propagation_policy(tag_propagation_policy_t::TPP_DONT);
}

void delay_cpu::set_dly(size_t d)
{
    // only set a new delta if there is a change in the delay; this
    // protects from quickly-repeated calls to this function that
    // would end with d_delta=0.
    if (d != dly()) {
        int old = dly();
        d_delay = d;
        d_delta += dly() - old;
    }
}

work_return_code_t delay_cpu::work(work_io& wio)
{
    auto itemsize = wio.outputs()[0].buf().item_size();

    const uint8_t* iptr;
    uint8_t* optr;
    int cons, ret;
    int noutput_items = std::min(wio.outputs()[0].n_items,
                                 wio.inputs()[0].n_items); // - (dly() - d_delta);

    // No change in delay; just memcpy ins to outs
    if (d_delta == 0) {
        for (size_t i = 0; i < wio.inputs().size(); i++) {
            iptr = wio.inputs()[i].items<uint8_t>();
            optr = wio.outputs()[i].items<uint8_t>();
            std::memcpy(optr, iptr, noutput_items * itemsize);
        }
        cons = noutput_items;
        ret = noutput_items;
    }

    // Skip over d_delta items on the input
    else if (d_delta < 0) {
        int n_to_copy, n_adj;
        int delta = -d_delta;
        n_to_copy = std::max(0, noutput_items - delta);
        n_adj = std::min(delta, noutput_items);
        for (size_t i = 0; i < wio.inputs().size(); i++) {
            iptr = wio.inputs()[i].items<uint8_t>();
            optr = wio.outputs()[i].items<uint8_t>();
            std::memcpy(optr, iptr + delta * itemsize, n_to_copy * itemsize);
        }
        cons = noutput_items;
        ret = n_to_copy;
        delta -= n_adj;
        d_delta = -delta;
    }

    // produce but not consume (inserts zeros)
    else { // d_delta > 0
        int n_from_input, n_padding;
        n_from_input = std::max(0, noutput_items - d_delta);
        n_padding = std::min(d_delta, noutput_items);
        for (size_t i = 0; i < wio.inputs().size(); i++) {
            iptr = wio.inputs()[i].items<uint8_t>();
            optr = wio.outputs()[i].items<uint8_t>();
            std::memset(optr, 0, n_padding * itemsize);
            std::memcpy(optr + n_padding * itemsize, iptr, n_from_input * itemsize);
        }
        cons = n_from_input;
        ret = noutput_items;
        d_delta -= n_padding;
    }

    // Delay the tags - which are not propagated automatically
    for (size_t i = 0; i < wio.inputs().size(); i++) {
        auto& inp = wio.inputs()[i];
        auto& out = wio.outputs()[i];
        auto nw = out.nitems_written();
        auto tags = inp.tags_in_window(0, noutput_items);
        for (auto& t : tags) {
            if (t.offset() + d_delay < nw + ret) {
                t.set_offset(t.offset() + d_delay);
                out.add_tag(t);
            }
        }
    }

    wio.consume_each(cons);
    wio.produce_each(ret);

    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr
