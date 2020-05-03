/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pwr_squelch_cc_impl.h"

namespace gr {
namespace analog {

pwr_squelch_cc::sptr
pwr_squelch_cc::make(double threshold, double alpha, int ramp, bool gate)
{
    return gnuradio::make_block_sptr<pwr_squelch_cc_impl>(threshold, alpha, ramp, gate);
}

pwr_squelch_cc_impl::pwr_squelch_cc_impl(double threshold,
                                         double alpha,
                                         int ramp,
                                         bool gate)
    : block("pwr_squelch_cc",
            io_signature::make(1, 1, sizeof(gr_complex)),
            io_signature::make(1, 1, sizeof(gr_complex))),
      squelch_base_cc_impl("pwr_squelch_cc", ramp, gate),
      d_pwr(0),
      d_iir(alpha)
{
    set_threshold(threshold);
}

pwr_squelch_cc_impl::~pwr_squelch_cc_impl() {}

std::vector<float> pwr_squelch_cc_impl::squelch_range() const
{
    std::vector<float> r(3);
    r[0] = -50.0;               // min	FIXME
    r[1] = +50.0;               // max	FIXME
    r[2] = (r[1] - r[0]) / 100; // step size

    return r;
}

void pwr_squelch_cc_impl::update_state(const gr_complex& in)
{
    d_pwr = d_iir.filter(in.real() * in.real() + in.imag() * in.imag());
}

void pwr_squelch_cc_impl::set_threshold(double db)
{
    gr::thread::scoped_lock l(d_setlock);
    d_threshold = std::pow(10.0, db / 10);
}

void pwr_squelch_cc_impl::set_alpha(double alpha)
{
    gr::thread::scoped_lock l(d_setlock);
    d_iir.set_taps(alpha);
}

} /* namespace analog */
} /* namespace gr */
