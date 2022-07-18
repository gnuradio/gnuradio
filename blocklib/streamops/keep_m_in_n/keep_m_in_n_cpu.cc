/* -*- c++ -*- */
/*
 * Copyright 2012,2018,2022 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "keep_m_in_n_cpu.h"
#include "keep_m_in_n_cpu_gen.h"

namespace gr {
namespace streamops {

keep_m_in_n_cpu::keep_m_in_n_cpu(block_args args) : INHERITED_CONSTRUCTORS
{
    // sanity checking
    if (args.m <= 0) {
        throw std::runtime_error(fmt::format("m={:d} but must be > 0", args.m));
    }
    if (args.n <= 0) {
        throw std::runtime_error(fmt::format("n={:d} but must be > 0", args.n));
    }
    if (args.m > args.n) {
        throw std::runtime_error(fmt::format("m = {:d} ≤ {:d} = n", args.m, args.n));
    }
    if (args.offset < 0) {
        throw std::runtime_error(
            fmt::format("offset {:d} but must be >= 0", args.offset));
    }
    if (args.offset >= args.n) {
        throw std::runtime_error(
            fmt::format("offset = {:d} < {:d} = n", args.offset, args.n));
    }

    set_output_multiple(args.m);

    // TODO: integer ratio relative rate
    // set_relative_rate(static_cast<uint64_t>(m), static_cast<uint64_t>(n));
    set_relative_rate((double)args.m / args.n);
}

work_return_code_t keep_m_in_n_cpu::work(work_io& wio)

{
    auto out = wio.outputs()[0].items<uint8_t>();
    auto in = wio.inputs()[0].items<uint8_t>();
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;

    // Grab our parameters
    auto itemsize = wio.outputs()[0].buf().item_size();
    auto m = pmtf::get_as<size_t>(*this->param_m);
    auto n = pmtf::get_as<size_t>(*this->param_n);
    auto offset = pmtf::get_as<size_t>(*this->param_offset);

    // iterate over data blocks of size {n, input : m, output}
    int blks = std::min(noutput_items / m, ninput_items / n);
    int excess = (offset + m - n) * itemsize;

    for (int i = 0; i < blks; i++) {
        // set up copy pointers
        const uint8_t* iptr = &in[(i * n + offset) * itemsize];
        uint8_t* optr = &out[i * m * itemsize];
        // perform copy
        if (excess <= 0) {
            memcpy(optr, iptr, m * itemsize);
        }
        else {
            memcpy(optr, &in[i * n * itemsize], excess);
            memcpy(optr + excess, iptr, m * itemsize - excess);
        }
    }

    wio.consume_each(blks * n);
    wio.produce_each(blks * m);
    return work_return_code_t::WORK_OK;
}

void keep_m_in_n_cpu::on_parameter_change(param_action_sptr action)
{
    // This will set the underlying PMT
    block::on_parameter_change(action);

    auto m = pmtf::get_as<size_t>(*this->param_m);
    auto n = pmtf::get_as<size_t>(*this->param_n);

    // Do more updating for certain parameters
    if (action->id() == keep_m_in_n::id_m) {

        set_output_multiple(m);
        set_relative_rate((double)m / n);
    }
    else if (action->id() == keep_m_in_n::id_n) {
        set_relative_rate((double)m / n);
    }
}
} // namespace streamops
} // namespace gr