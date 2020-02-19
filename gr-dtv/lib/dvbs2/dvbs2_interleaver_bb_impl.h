/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_INTERLEAVER_BB_IMPL_H
#define INCLUDED_DTV_DVBS2_INTERLEAVER_BB_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbs2_interleaver_bb.h>

namespace gr {
namespace dtv {

class dvbs2_interleaver_bb_impl : public dvbs2_interleaver_bb
{
private:
    int frame_size;
    int signal_constellation;
    int code_rate;
    int mod;
    int packed_items;
    int rowaddr0;
    int rowaddr1;
    int rowaddr2;
    int rowaddr3;
    int rowaddr4;
    int rowaddr5;
    int rowaddr6;
    int rowaddr7;

public:
    dvbs2_interleaver_bb_impl(dvb_framesize_t framesize,
                              dvb_code_rate_t rate,
                              dvb_constellation_t constellation);
    ~dvbs2_interleaver_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_INTERLEAVER_BB_IMPL_H */
