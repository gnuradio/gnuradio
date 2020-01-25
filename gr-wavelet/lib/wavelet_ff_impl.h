/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_WAVELET_WAVELET_FF_IMPL_H
#define INCLUDED_WAVELET_WAVELET_FF_IMPL_H

#include <gnuradio/wavelet/wavelet_ff.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_wavelet.h>

namespace gr {
namespace wavelet {

class WAVELET_API wavelet_ff_impl : public wavelet_ff
{
    int d_size;
    int d_order;
    bool d_forward;
    gsl_wavelet* d_wavelet;
    gsl_wavelet_workspace* d_workspace;
    double* d_temp;

public:
    wavelet_ff_impl(int size, int order, bool forward);

    ~wavelet_ff_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WAVELET_FF_IMPL_H */
