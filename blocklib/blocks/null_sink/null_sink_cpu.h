/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/blocks/null_sink.h>

namespace gr {
namespace blocks {

class null_sink_cpu : public null_sink
{
public:
    null_sink_cpu(block_args args);
    work_return_code_t work(work_io&) override;

protected:
    size_t d_nports;
};

} // namespace blocks
} // namespace gr
