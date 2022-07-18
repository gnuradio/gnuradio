/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "selector_cpu.h"
#include "selector_cpu_gen.h"

namespace gr {
namespace streamops {

selector_cpu::selector_cpu(block_args args)
    : INHERITED_CONSTRUCTORS,
      d_itemsize(args.itemsize),
      d_num_inputs(args.num_inputs),
      d_num_outputs(args.num_outputs)
{
    set_tag_propagation_policy(gr::tag_propagation_policy_t::TPP_CUSTOM);
}

work_return_code_t selector_cpu::work(work_io& wio)

{
    auto input_index = pmtf::get_as<size_t>(*this->param_input_index);
    auto output_index = pmtf::get_as<size_t>(*this->param_output_index);
    auto enabled = pmtf::get_as<bool>(*this->param_enabled);
    auto in = wio.inputs()[input_index].items<uint8_t>();
    auto out = wio.outputs()[output_index].items<uint8_t>();
    auto noutput_items =
        std::min(wio.inputs()[input_index].n_items, wio.outputs()[output_index].n_items);

    if (d_itemsize == 0) {
        d_itemsize = wio.outputs()[output_index].buf().item_size();
    }

    if (enabled) {
        auto nread = wio.inputs()[input_index].nitems_read();
        auto nwritten = wio.outputs()[output_index].nitems_written();

        auto tags =
            wio.inputs()[input_index].tags_in_window(nread, nread + noutput_items);

        for (auto tag : tags) {
            tag.set_offset(tag.offset() - (nread - nwritten));
            wio.outputs()[output_index].add_tag(tag);
        }

        std::copy(in, in + noutput_items * d_itemsize, out);
        wio.outputs()[output_index].n_produced = noutput_items;
    }

    wio.consume_each(noutput_items);
    return work_return_code_t::WORK_OK;
}


} // namespace streamops
} // namespace gr