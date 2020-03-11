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

namespace gr {
namespace digital {

class constellation_soft_decoder_cf_impl : public constellation_soft_decoder_cf
{
private:
    constellation_sptr d_constellation;
    unsigned int d_dim;
    int d_bps;

public:
    constellation_soft_decoder_cf_impl(constellation_sptr constellation);
    ~constellation_soft_decoder_cf_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CONSTELLATION_SOFT_DECODER_CF_IMPL_H */
