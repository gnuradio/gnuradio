/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_IMPL_H
#define INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_IMPL_H

#include "dvbt_configure.h"
#include <gnuradio/dtv/dvbt_bit_inner_deinterleaver.h>

namespace gr {
namespace dtv {

class dvbt_bit_inner_deinterleaver_impl : public dvbt_bit_inner_deinterleaver
{
private:
    const dvbt_configure config;

    static const int d_lookup_H[126][6];

    const int d_nsize;
    dvbt_hierarchy_t d_hierarchy;

    // constellation
    const int d_v;
    // Bit interleaver block size
    static const int d_bsize;

    // Table to keep interleaved indices
    std::vector<unsigned char> d_perm;

public:
    dvbt_bit_inner_deinterleaver_impl(int nsize,
                                      dvb_constellation_t constellation,
                                      dvbt_hierarchy_t hierarchy,
                                      dvbt_transmission_mode_t transmission);
    ~dvbt_bit_inner_deinterleaver_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_BIT_INNER_DEINTERLEVER_IMPL_H */
