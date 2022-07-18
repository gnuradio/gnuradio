/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "stream_to_streams_cuda.h"
#include "stream_to_streams_cuda_gen.h"
#include <gnuradio/helper_cuda.h>
#include <volk/volk.h>

namespace gr {
namespace streamops {

stream_to_streams_cuda::stream_to_streams_cuda(const block_args& args)
    : INHERITED_CONSTRUCTORS, d_nstreams(args.nstreams)
{
    d_out_items.resize(args.nstreams);
    cudaStreamCreate(&d_stream);
}

work_return_code_t stream_to_streams_cuda::work(work_io& wio)

{
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;
    size_t nstreams = wio.outputs().size();
    auto itemsize = wio.outputs()[0].buf().item_size();

    if (!p_kernel) {
        p_kernel =
            std::make_shared<cusp::deinterleave>((int)d_nstreams, 1, (int)itemsize);
        p_kernel->set_stream(d_stream);
    }


    auto total_items = std::min(ninput_items / nstreams, (size_t)noutput_items);

    d_out_items = wio.all_output_ptrs();

    p_kernel->launch_default_occupancy({ wio.inputs()[0].items<uint8_t>() },
                                       d_out_items,
                                       itemsize * total_items * nstreams);
    cudaStreamSynchronize(d_stream);

    wio.produce_each(total_items);
    wio.consume_each(total_items * nstreams);
    return work_return_code_t::WORK_OK;
}


} /* namespace streamops */
} /* namespace gr */
