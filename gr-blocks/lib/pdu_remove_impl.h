/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H
#define INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H

#include <gnuradio/blocks/pdu_remove.h>

namespace gr {
namespace blocks {

class pdu_remove_impl : public pdu_remove
{
private:
    pmt::pmt_t d_k;

public:
    pdu_remove_impl(pmt::pmt_t k);
    void handle_msg(pmt::pmt_t msg);
    void set_key(pmt::pmt_t key) { d_k = key; };
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_REMOVE_IMPL_H */
