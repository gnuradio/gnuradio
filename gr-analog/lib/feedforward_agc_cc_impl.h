/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_IMPL_H
#define INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_IMPL_H

#include <gnuradio/analog/feedforward_agc_cc.h>

namespace gr {
namespace analog {

class feedforward_agc_cc_impl : public feedforward_agc_cc
{
private:
    int d_nsamples;
    float d_reference;

public:
    feedforward_agc_cc_impl(int nsamples, float reference);
    ~feedforward_agc_cc_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FEEDFORWARD_AGC_CC_IMPL_H */
