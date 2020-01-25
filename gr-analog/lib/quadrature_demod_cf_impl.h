/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H
#define INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H

#include <gnuradio/analog/quadrature_demod_cf.h>

namespace gr {
namespace analog {

class quadrature_demod_cf_impl : public quadrature_demod_cf
{
private:
    float d_gain;

public:
    quadrature_demod_cf_impl(float gain);
    ~quadrature_demod_cf_impl();

    void set_gain(float gain) { d_gain = gain; }
    float gain() const { return d_gain; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_IMPL_H */
