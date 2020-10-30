/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H
#define INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H

#include <gnuradio/analog/probe_avg_mag_sqrd_c.h>
#include <gnuradio/filter/single_pole_iir.h>

namespace gr {
namespace analog {

class probe_avg_mag_sqrd_c_impl : public probe_avg_mag_sqrd_c
{
private:
    double d_threshold;
    bool d_unmuted;
    double d_level;
    filter::single_pole_iir<double, double, double> d_iir;

public:
    probe_avg_mag_sqrd_c_impl(double threshold_db, double alpha = 0.0001);
    ~probe_avg_mag_sqrd_c_impl() override;

    bool unmuted() const override { return d_unmuted; }
    double level() const override { return d_level; }

    double threshold() const override;

    void set_alpha(double alpha) override;
    void set_threshold(double decibels) override;
    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PROBE_AVG_MAG_SQRD_C_IMPL_H */
