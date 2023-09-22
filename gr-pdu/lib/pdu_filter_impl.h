/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_FILTER_IMPL_H
#define INCLUDED_PDU_PDU_FILTER_IMPL_H

#include <gnuradio/pdu/pdu_filter.h>

namespace gr {
namespace pdu {

class pdu_filter_impl : public pdu_filter
{
private:
    pmt::pmt_t d_k;
    pmt::pmt_t d_v;
    bool d_invert;

public:
    pdu_filter_impl(pmt::pmt_t k, pmt::pmt_t v, bool invert);
    void handle_msg(pmt::pmt_t msg);
    void set_key(pmt::pmt_t key) override { d_k = key; };
    void set_val(pmt::pmt_t val) override { d_v = val; };
    void set_inversion(bool invert) override { d_invert = invert; };
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_PDU_FILTER_IMPL_H */
