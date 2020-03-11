/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_IMPL_H
#define INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_IMPL_H

#include <gnuradio/dtv/dvbt_energy_dispersal.h>

namespace gr {
namespace dtv {

class dvbt_energy_dispersal_impl : public dvbt_energy_dispersal
{
private:
    // Packet size
    static const int d_psize;
    // Number of packets after which PRBS is reset
    static const int d_npacks;
    // Number of blocks
    int d_nblocks;
    // SYNC value
    static const int d_SYNC;
    // Negative SYNC value
    static const int d_NSYNC;

    // Register for PRBS
    int d_reg;

    void init_prbs();
    int clock_prbs(int clocks);

public:
    dvbt_energy_dispersal_impl(int nsize);
    ~dvbt_energy_dispersal_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DISPERSAL_IMPL_H */
