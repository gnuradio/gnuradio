/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/stream_to_streams.h>
#include <cusp/deinterleave.cuh>

namespace gr {
namespace streamops {

class stream_to_streams_cuda : public stream_to_streams
{
public:
    stream_to_streams_cuda(const block_args& args);

    work_return_code_t work(work_io&) override;

private:
    size_t d_nstreams;

    std::vector<void*> d_out_items;

    std::shared_ptr<cusp::deinterleave> p_kernel;
    cudaStream_t d_stream;
};


} // namespace streamops
} // namespace gr
