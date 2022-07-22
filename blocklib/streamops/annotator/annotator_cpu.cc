/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "annotator_cpu.h"
#include "annotator_cpu_gen.h"
#include <pmtf/scalar.hpp>
#include <pmtf/string.hpp>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace gr {
namespace streamops {

annotator_cpu::annotator_cpu(const block_args& args)
    : INHERITED_CONSTRUCTORS,
      d_when(args.when),
      d_num_inputs(args.num_inputs),
      d_num_outputs(args.num_outputs),
      d_tpp(args.tpp)
{

    set_tag_propagation_policy(args.tpp);

    d_tag_counter = 0;
    // set_relative_rate(1, 1);
}

work_return_code_t annotator_cpu::work(work_io& wio)

{
    auto noutput_items = wio.outputs()[0].n_items;

    uint64_t abs_N = 0;

    for (unsigned i = 0; i < d_num_inputs; i++) {
        abs_N = wio.inputs()[i].nitems_read();

        auto tags = wio.inputs()[i].tags_in_window(0, noutput_items);
        d_stored_tags.insert(d_stored_tags.end(), tags.begin(), tags.end());
    }

    // Storing the current noutput_items as the value to the "noutput_items" key
    auto srcid = pmtf::string(alias());
    auto key = "seq";

    // Work does nothing to the data stream; just copy all inputs to outputs
    // Adds a new tag when the number of items read is a multiple of d_when
    abs_N = wio.outputs()[0].buf().total_written();

    for (size_t j = 0; j < noutput_items; j++) {
        // the min() is a hack to make sure this doesn't segfault if
        // there are a different number of ins and outs. This is
        // specifically designed to test the 1-to-1 propagation policy.
        // for (unsigned i = 0; i < std::min(d_num_outputs, d_num_inputs); i++) {
        for (unsigned i = 0; i < d_num_outputs; i++) {
            if (abs_N % d_when == 0) {
                auto value = pmtf::scalar<uint64_t>(d_tag_counter++);
                // tag_map tm = {{key, value}, {"srcid",srcid}};
                wio.outputs()[i].buf().add_tag(
                    abs_N, pmtf::map{ { key, value }, { "srcid", srcid } });
            }
        }
        abs_N++;
    }
    for (unsigned i = 0; i < d_num_outputs; i++) {
        wio.outputs()[i].n_produced = noutput_items;
    }

    return work_return_code_t::WORK_OK;
}

} /* namespace streamops */
} /* namespace gr */
