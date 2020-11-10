/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_DVBT_DEMAP_IMPL_H
#define INCLUDED_DTV_DVBT_DVBT_DEMAP_IMPL_H

#include "dvbt_configure.h"
#include <gnuradio/dtv/dvbt_demap.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace dtv {

class dvbt_demap_impl : public dvbt_demap
{
private:
    const dvbt_configure config;

    int d_nsize;

    // Constellation size
    const unsigned char d_constellation_size;
    // Step on each axis of the constellation
    unsigned char d_step;
    // Keep Alpha internally
    unsigned char d_alpha;
    // Gain for the complex values
    float d_gain;

    volk::vector<gr_complex> d_constellation_points;
    volk::vector<float> d_sq_dist;

    void make_constellation_points(int size, int step, int alpha);
    int find_constellation_value(gr_complex val);
    int bin_to_gray(int val);

public:
    dvbt_demap_impl(int nsize,
                    dvb_constellation_t constellation,
                    dvbt_hierarchy_t hierarchy,
                    dvbt_transmission_mode_t transmission,
                    float gain);
    ~dvbt_demap_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_DVBT_DEMAP_IMPL_H */
