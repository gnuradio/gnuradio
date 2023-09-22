/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_FPLL_IMPL_H
#define INCLUDED_DTV_ATSC_FPLL_IMPL_H

#include <gnuradio/analog/agc.h>
#include <gnuradio/dtv/atsc_fpll.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/nco.h>
#include <cstdio>

namespace gr {
namespace dtv {

class atsc_fpll_impl : public atsc_fpll
{
private:
    gr::nco<float, float> d_nco;
    gr::filter::single_pole_iir<gr_complex, gr_complex, float> d_afc;

public:
    atsc_fpll_impl(float rate);
    ~atsc_fpll_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_FPLL_IMPL_H */
