/* -*- c++ -*- */
/*
 * Copyright 2004-2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SINGLE_POLE_IIR_FILTER_CC_IMPL_H
#define INCLUDED_SINGLE_POLE_IIR_FILTER_CC_IMPL_H

#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/filter/single_pole_iir_filter_cc.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/sync_block.h>
#include <stdexcept>

namespace gr {
namespace filter {

class FILTER_API single_pole_iir_filter_cc_impl : public single_pole_iir_filter_cc
{
private:
    unsigned int d_vlen;
    std::vector<single_pole_iir<gr_complex, gr_complex, double>> d_iir;

public:
    single_pole_iir_filter_cc_impl(double alpha, unsigned int vlen);

    void set_taps(double alpha) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_FILTER_CC_IMPL_H */
