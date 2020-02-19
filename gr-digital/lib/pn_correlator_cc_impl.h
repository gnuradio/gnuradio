/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H
#define INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H

#include <gnuradio/digital/glfsr.h>
#include <gnuradio/digital/pn_correlator_cc.h>

namespace gr {
namespace digital {

class pn_correlator_cc_impl : public pn_correlator_cc
{
private:
    int d_len;
    float d_pn;
    glfsr* d_reference;

public:
    pn_correlator_cc_impl(int degree, int mask = 0, int seed = 1);
    ~pn_correlator_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PN_CORRELATOR_CC_IMPL_H */
