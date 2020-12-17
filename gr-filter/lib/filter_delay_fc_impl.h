/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FILTER_DELAY_FC_IMPL_H
#define INCLUDED_FILTER_FILTER_DELAY_FC_IMPL_H

#include <gnuradio/filter/filter_delay_fc.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/io_signature.h>

namespace gr {
namespace filter {

class FILTER_API filter_delay_fc_impl : public filter_delay_fc
{
private:
    std::vector<float> d_taps;
    kernel::fir_filter_fff d_fir;
    unsigned int d_delay;
    bool d_update;

public:
    filter_delay_fc_impl(const std::vector<float>& taps);

    std::vector<float> taps();
    void set_taps(const std::vector<float>& taps);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FILTER_DELAY_FC_IMPL_H */
