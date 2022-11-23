/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/blocks/message_debug.h>

namespace gr {
namespace blocks {

class message_debug_cpu : public virtual message_debug
{
public:
    message_debug_cpu(block_args args);
    size_t num_messages() override { return d_messages.size(); }
    pmtv::pmt get_message(size_t i) override { return d_messages[i]; }

private:
    std::vector<pmtv::pmt> d_messages;
    void handle_msg_print(pmtv::pmt msg) override;
    void handle_msg_store(pmtv::pmt msg) override;
};

} // namespace blocks
} // namespace gr
