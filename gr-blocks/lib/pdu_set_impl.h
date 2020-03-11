/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_PDU_SET_IMPL_H
#define INCLUDED_BLOCKS_PDU_SET_IMPL_H

#include <gnuradio/blocks/pdu_set.h>

namespace gr {
namespace blocks {

class pdu_set_impl : public pdu_set
{
private:
    pmt::pmt_t d_k;
    pmt::pmt_t d_v;

public:
    pdu_set_impl(pmt::pmt_t k, pmt::pmt_t v);
    void handle_msg(pmt::pmt_t msg);
    void set_key(pmt::pmt_t key) { d_k = key; };
    void set_val(pmt::pmt_t val) { d_v = val; };
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_PDU_SET_IMPL_H */
