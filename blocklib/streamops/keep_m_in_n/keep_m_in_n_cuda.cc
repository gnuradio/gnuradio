/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "keep_m_in_n_cuda.h"
#include "keep_m_in_n_cuda_gen.h"

#include <gnuradio/helper_cuda.h>

#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>


namespace gr {
namespace streamops {

keep_m_in_n_cuda::keep_m_in_n_cuda(block_args args) : INHERITED_CONSTRUCTORS

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
    if (args.offset >= args.n) {
        throw std::runtime_error(
            fmt::format("offset = {:d} < {:d} = n", args.offset, args.n));
    }


    // can change this to uint64_t by restricting the output multiple
    p_kernel = std::make_shared<cusp::keep_m_in_n<uint8_t>>(args.m, args.n, 4);
    p_kernel->occupancy(&d_min_block, &d_min_grid);

    cudaStreamCreate(&d_stream);
    p_kernel->set_stream(d_stream);

    set_output_multiple(args.m);
    set_relative_rate((double)args.m / args.n);
}

work_return_code_t keep_m_in_n_cuda::work(work_io& wio)

{
    auto out = wio.outputs()[0].items<uint8_t>();
    auto in = wio.inputs()[0].items<uint8_t>();
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;
    auto itemsize = wio.outputs()[0].buf().item_size();
    auto m = pmtf::get_as<size_t>(*this->param_m);
    auto n = pmtf::get_as<size_t>(*this->param_n);
    auto offset = pmtf::get_as<size_t>(*this->param_offset);

    // iterate over data blocks of size {n, input : m, output}
    int blks = std::min(noutput_items / m, ninput_items / n);

    if (blks == 0) {
        wio.consume_each(0);
        wio.produce_each(0);
        return work_return_code_t::WORK_OK;
    }

    int gridSize = (blks * m * itemsize + d_min_block - 1) / d_min_block;
    checkCudaErrors(p_kernel->launch(in,
                                     out,
                                     m,
                                     n,
                                     itemsize,
                                     offset,
                                     gridSize,
                                     d_min_block,
                                     blks * m * itemsize,
                                     d_stream));

    wio.consume_each(blks * n);
    wio.produce_each(blks * m);
    return work_return_code_t::WORK_OK;
}

void keep_m_in_n_cuda::on_parameter_change(param_action_sptr action)
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
