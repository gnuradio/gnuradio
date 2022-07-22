/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "message_debug_cpu.h"
#include "message_debug_cpu_gen.h"

namespace gr {
namespace blocks {

message_debug_cpu::message_debug_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

void message_debug_cpu::handle_msg_print(pmtf::pmt msg)
{
    std::cout << "PMT printing not yet implemented" << std::endl;
}

void message_debug_cpu::handle_msg_store(pmtf::pmt msg) { d_messages.push_back(msg); }

} // namespace blocks
} // namespace gr
