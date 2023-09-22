/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_PDU_REMOVE_IMPL_H
#define INCLUDED_PDU_PDU_REMOVE_IMPL_H

#include <gnuradio/pdu/pdu_remove.h>

namespace gr {
namespace pdu {

class pdu_remove_impl : public pdu_remove
{
private:
    pmt::pmt_t d_k;

public:
    pdu_remove_impl(pmt::pmt_t k);
    void handle_msg(pmt::pmt_t msg);
    void set_key(pmt::pmt_t key) override { d_k = key; };
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_PDU_REMOVE_IMPL_H */
