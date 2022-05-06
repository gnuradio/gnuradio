/* -*- c++ -*- */
/*
 * Copyright 2021 Cesar Martinez.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC16_ASYNC_BB_IMPL_H
#define INCLUDED_DIGITAL_CRC16_ASYNC_BB_IMPL_H

#include <gnuradio/digital/crc.h>
#include <gnuradio/digital/crc16_async_bb.h>

namespace gr {
namespace digital {

class crc16_async_bb_impl : public crc16_async_bb
{
private:
    crc d_crc_ccitt_impl;

    pmt::pmt_t d_in_port;
    pmt::pmt_t d_out_port;

    void calc(pmt::pmt_t msg);
    void check(pmt::pmt_t msg);

public:
    crc16_async_bb_impl(bool check);
    ~crc16_async_bb_impl() override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    uint64_t d_npass;
    uint64_t d_nfail;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_crc16_ASYNC_BB_IMPL_H */
