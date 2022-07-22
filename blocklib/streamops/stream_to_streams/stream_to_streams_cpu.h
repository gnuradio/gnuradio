/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/streamops/stream_to_streams.h>

namespace gr {
namespace streamops {

class stream_to_streams_cpu : public stream_to_streams
{
public:
    stream_to_streams_cpu(const block_args& args);

    work_return_code_t work(work_io&) override;
};


} // namespace streamops
} // namespace gr
