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

#include "pwr_squelch_ff_impl.h"

namespace gr {
namespace analog {

pwr_squelch_ff::sptr
pwr_squelch_ff::make(double threshold, double alpha, int ramp, bool gate)
{
    return gnuradio::make_block_sptr<pwr_squelch_ff_impl>(threshold, alpha, ramp, gate);
}

pwr_squelch_ff_impl::pwr_squelch_ff_impl(double threshold,
                                         double alpha,
                                         int ramp,
                                         bool gate)
    : block("pwr_squelch_ff",
            io_signature::make(1, 1, sizeof(float)),
            io_signature::make(1, 1, sizeof(float))),
      squelch_base_ff_impl("pwr_squelch_ff", ramp, gate),
      d_pwr(0),
      d_iir(alpha)
{
    set_threshold(threshold);
}

pwr_squelch_ff_impl::~pwr_squelch_ff_impl() {}

std::vector<float> pwr_squelch_ff_impl::squelch_range() const
{
    std::vector<float> r(3);
    r[0] = -50.0;               // min	FIXME
    r[1] = +50.0;               // max	FIXME
    r[2] = (r[1] - r[0]) / 100; // step size

    return r;
}

void pwr_squelch_ff_impl::update_state(const float& in) { d_pwr = d_iir.filter(in * in); }

} /* namespace analog */
} /* namespace gr */
