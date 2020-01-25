/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_HILBERT_FC_IMPL_H
#define INCLUDED_FILTER_HILBERT_FC_IMPL_H

#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/hilbert_fc.h>
#include <gnuradio/types.h>

namespace gr {
namespace filter {

class FILTER_API hilbert_fc_impl : public hilbert_fc
{
private:
    unsigned int d_ntaps;
    kernel::fir_filter_fff* d_hilb;

public:
    hilbert_fc_impl(unsigned int ntaps,
                    firdes::win_type window = firdes::WIN_HAMMING,
                    double beta = 6.76);

    ~hilbert_fc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_HILBERT_FC_IMPL_H */
