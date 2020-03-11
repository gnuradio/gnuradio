/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_PWR_SQUELCH_CC_IMPL_H
#define INCLUDED_ANALOG_PWR_SQUELCH_CC_IMPL_H

#include "squelch_base_cc_impl.h"
#include <gnuradio/analog/pwr_squelch_cc.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <cmath>

namespace gr {
namespace analog {

class ANALOG_API pwr_squelch_cc_impl : public pwr_squelch_cc, squelch_base_cc_impl
{
private:
    double d_threshold;
    double d_pwr;
    filter::single_pole_iir<double, double, double> d_iir;

protected:
    virtual void update_state(const gr_complex& in);
    virtual bool mute() const { return d_pwr < d_threshold; }

public:
    pwr_squelch_cc_impl(double db,
                        double alpha = 0.0001,
                        int ramp = 0,
                        bool gate = false);
    ~pwr_squelch_cc_impl();

    std::vector<float> squelch_range() const;

    double threshold() const { return 10 * log10(d_threshold); }
    void set_threshold(double db);
    void set_alpha(double alpha);

    int ramp() const { return squelch_base_cc_impl::ramp(); }
    void set_ramp(int ramp) { squelch_base_cc_impl::set_ramp(ramp); }
    bool gate() const { return squelch_base_cc_impl::gate(); }
    void set_gate(bool gate) { squelch_base_cc_impl::set_gate(gate); }
    bool unmuted() const { return squelch_base_cc_impl::unmuted(); }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
    {
        return squelch_base_cc_impl::general_work(
            noutput_items, ninput_items, input_items, output_items);
    }
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_PWR_SQUELCH_CC_IMPL_H */
