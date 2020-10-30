/* -*- c++ -*- */
/*
 * Copyright 2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_IMPL_H
#define INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_IMPL_H

#include <gnuradio/analog/simple_squelch_cc.h>
#include <gnuradio/filter/single_pole_iir.h>

namespace gr {
namespace analog {

class simple_squelch_cc_impl : public simple_squelch_cc
{
private:
    double d_threshold;
    bool d_unmuted;
    filter::single_pole_iir<double, double, double> d_iir;

public:
    simple_squelch_cc_impl(double threshold_db, double alpha);
    ~simple_squelch_cc_impl() override;

    bool unmuted() const override { return d_unmuted; }

    void set_alpha(double alpha) override;
    void set_threshold(double decibels) override;

    double threshold() const override;
    std::vector<float> squelch_range() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SIMPLE_SQUELCH_CC_IMPL_H */
