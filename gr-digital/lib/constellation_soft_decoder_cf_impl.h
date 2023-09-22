/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_IMPL_H
#define INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_IMPL_H

#include <gnuradio/digital/constellation_soft_decoder_cf.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace digital {

class constellation_soft_decoder_cf_impl : public constellation_soft_decoder_cf
{
private:
    constellation_sptr d_constellation;
    unsigned int d_dim;
    unsigned int d_bps;
    float d_npwr;
    gr::thread::mutex d_mutex;
    bool d_warned_bps = false; // To record if a bps change warning has been raised, and
                               // limit logging spam

public:
    constellation_soft_decoder_cf_impl(constellation_sptr constellation,
                                       float npwr = 1.0);
    ~constellation_soft_decoder_cf_impl() override;

    void set_npwr(float npwr) override;
    void set_constellation(constellation_sptr constellation) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_IMPL_H */
