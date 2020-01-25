/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H
#define INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H

#include <gnuradio/dtv/dvbt_energy_descramble.h>

namespace gr {
namespace dtv {

class dvbt_energy_descramble_impl : public dvbt_energy_descramble
{
private:
    static const int d_nblocks;
    static const int d_bsize;
    static const int d_SYNC;
    static const int d_NSYNC;
    static const int d_MUX_PKT;

    // Register for PRBS
    int d_reg;

    // Index
    int d_index;
    // Search interval
    int d_search;

    void init_prbs();
    int clock_prbs(int clocks);

public:
    dvbt_energy_descramble_impl(int nblocks);
    ~dvbt_energy_descramble_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H */
