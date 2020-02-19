/* -*- c++ -*- */
/*
 * Copyright 2015,2019 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H
#define INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H

#include "dvb_defines.h"
#include <gnuradio/dtv/dvb_bbscrambler_bb.h>

namespace gr {
namespace dtv {

class dvb_bbscrambler_bb_impl : public dvb_bbscrambler_bb
{
private:
    int kbch;
    int frame_size;
    unsigned char bb_randomise[FRAME_SIZE_NORMAL];
    uint32_t* bb_randomize32;
    uint64_t* bb_randomize64;
    void init_bb_randomiser(void);

public:
    dvb_bbscrambler_bb_impl(dvb_standard_t standard,
                            dvb_framesize_t framesize,
                            dvb_code_rate_t rate);
    ~dvb_bbscrambler_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BBSCRAMBLER_BB_IMPL_H */
