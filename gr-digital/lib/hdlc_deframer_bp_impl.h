/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_IMPL_H
#define INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_IMPL_H

#include <gnuradio/digital/hdlc_deframer_bp.h>

namespace gr {
namespace digital {

class hdlc_deframer_bp_impl : public hdlc_deframer_bp
{
private:
    const size_t d_length_min;
    const size_t d_length_max;
    size_t d_ones = 0;
    size_t d_bytectr = 0;
    size_t d_bitctr = 0;
    std::vector<unsigned char> d_pktbuf;

    const pmt::pmt_t d_port;

public:
    hdlc_deframer_bp_impl(int length_min, int length_max);
    ~hdlc_deframer_bp_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_HDLC_DEFRAMER_BP_IMPL_H */
