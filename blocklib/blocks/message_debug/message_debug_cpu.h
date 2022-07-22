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
    pmtf::pmt get_message(size_t i) override { return d_messages[i]; }

private:
    std::vector<pmtf::pmt> d_messages;
    void handle_msg_print(pmtf::pmt msg) override;
    void handle_msg_store(pmtf::pmt msg) override;
};

} // namespace blocks
} // namespace gr
