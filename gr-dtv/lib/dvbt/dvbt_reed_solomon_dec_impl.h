/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_IMPL_H
#define INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_IMPL_H

#include <gnuradio/dtv/dvbt_reed_solomon_dec.h>

extern "C" {
#include <gnuradio/fec/rs.h>
}

namespace gr {
namespace dtv {

class dvbt_reed_solomon_dec_impl : public dvbt_reed_solomon_dec
{
private:
    int d_n;
    int d_k;
    int d_s;
    int d_blocks;

    int d_nerrors_corrected_count;
    int d_bad_packet_count;
    int d_total_packets;

    void* d_rs; /* Reed-Solomon characteristics structure */
    int decode(unsigned char& out, const unsigned char& in);

public:
    dvbt_reed_solomon_dec_impl(
        int p, int m, int gfpoly, int n, int k, int t, int s, int blocks);
    ~dvbt_reed_solomon_dec_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_REED_SOLOMON_DEC_IMPL_H */
