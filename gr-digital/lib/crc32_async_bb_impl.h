/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H
#define INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H

#include <gnuradio/digital/crc32_async_bb.h>
#include <boost/crc.hpp>

namespace gr {
namespace digital {

class crc32_async_bb_impl : public crc32_async_bb
{
private:
    boost::crc_optimal<32, 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, true> d_crc_impl;

    pmt::pmt_t d_in_port;
    pmt::pmt_t d_out_port;

    void calc(pmt::pmt_t msg);
    void check(pmt::pmt_t msg);

public:
    crc32_async_bb_impl(bool check);
    ~crc32_async_bb_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    uint64_t d_npass;
    uint64_t d_nfail;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_CRC32_ASYNC_BB_IMPL_H */
