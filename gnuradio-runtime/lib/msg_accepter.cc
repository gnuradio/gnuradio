/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/msg_accepter.h>
#include <stdexcept>

namespace gr {

msg_accepter::msg_accepter() {}

msg_accepter::~msg_accepter()
{
    // NOP, required as virtual destructor
}

void msg_accepter::post(pmt::pmt_t which_port, pmt::pmt_t msg)
{
    // Notify derived class, handled case by case
    block* p = dynamic_cast<block*>(this);
    if (p) {
        p->_post(which_port, msg);
        return;
    }

    hier_block2* p2 = dynamic_cast<hier_block2*>(this);
    if (p2) {
        // FIXME do the right thing
        return;
    }

    throw std::runtime_error("unknown derived class");
}

} /* namespace gr */
