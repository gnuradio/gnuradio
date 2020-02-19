/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_RANDOMIZER_BB_IMPL_H
#define INCLUDED_DTV_CATV_RANDOMIZER_BB_IMPL_H

#include <gnuradio/dtv/catv_randomizer_bb.h>

namespace gr {
namespace dtv {

class catv_randomizer_bb_impl : public catv_randomizer_bb
{
private:
    unsigned char rseq[88 * 128];
    int offset, max_offset;
    void init_rand();

public:
    catv_randomizer_bb_impl(catv_constellation_t constellation);
    ~catv_randomizer_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_RANDOMIZER_BB_IMPL_H */
