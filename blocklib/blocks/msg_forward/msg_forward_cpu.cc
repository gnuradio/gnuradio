/* -*- c++ -*- */
/*
 * Copyright 2019 Bastian Bloessl <mail@bastibl.net>.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "msg_forward_cpu.h"
#include "msg_forward_cpu_gen.h"

namespace gr {
namespace blocks {

msg_forward_cpu::msg_forward_cpu(block_args args)
    : INHERITED_CONSTRUCTORS, d_max_messages(args.max_messages)
{
}

} // namespace blocks
} // namespace gr
