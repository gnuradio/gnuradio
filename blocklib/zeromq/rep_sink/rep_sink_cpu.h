/* -*- c++ -*- */
/*
 * Copyright 2013,2014,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "base.h"
#include <gnuradio/zeromq/rep_sink.h>

namespace gr {
namespace zeromq {

class rep_sink_cpu : public virtual rep_sink, public virtual base_sink
{
public:
    rep_sink_cpu(block_args args);
    work_return_code_t work(work_io&) override;
    std::string last_endpoint() const override { return base_sink::last_endpoint(); }

    // Since vsize can be set as 0, then inferred on flowgraph init, set it during start()
    bool start() override
    {
        set_vsize(this->input_stream_ports()[0]->itemsize());
        return rep_sink::start();
    }
};

} // namespace zeromq
} // namespace gr