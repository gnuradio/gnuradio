/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_WAVELET_WVPS_FF_IMPL_H
#define INCLUDED_WAVELET_WVPS_FF_IMPL_H

#include <gnuradio/wavelet/wvps_ff.h>

namespace gr {
namespace wavelet {

class WAVELET_API wvps_ff_impl : public wvps_ff
{
    int d_ilen;
    int d_olen;

public:
    wvps_ff_impl(int ilen);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WVPS_FF_IMPL_H */
