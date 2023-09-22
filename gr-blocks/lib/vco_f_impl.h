/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_VCO_F_IMPL_H
#define INCLUDED_GR_VCO_F_IMPL_H

#include <gnuradio/blocks/vco_f.h>
#include <gnuradio/fxpt_vco.h>

namespace gr {
namespace blocks {

class vco_f_impl : public vco_f
{
private:
    const double d_sampling_rate;
    const double d_sensitivity;
    const double d_amplitude;
    double d_k;
    gr::fxpt_vco d_vco;

public:
    vco_f_impl(double sampling_rate, double sensitivity, double amplitude);
    ~vco_f_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_VCO_F_H */
