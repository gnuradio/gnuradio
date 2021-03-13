/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wavelet_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

#include <cstdio>

// NB in this version, only Daubechies wavelets
// order is wavelet length, even, 2...20

namespace gr {
namespace wavelet {

wavelet_ff::sptr wavelet_ff::make(int size, int order, bool forward)
{
    return gnuradio::make_block_sptr<wavelet_ff_impl>(size, order, forward);
}

wavelet_ff_impl::wavelet_ff_impl(int size, int order, bool forward)
    : sync_block("wavelet_ff",
                 io_signature::make(1, 1, size * sizeof(float)),
                 io_signature::make(1, 1, size * sizeof(float))),
      d_size(size),
      d_order(order),
      d_forward(forward)
{
    d_wavelet = gsl_wavelet_alloc(gsl_wavelet_daubechies, d_order);
    if (d_wavelet == NULL)
        throw std::runtime_error("can't allocate wavelet");
    d_workspace = gsl_wavelet_workspace_alloc(d_size);
    if (d_workspace == NULL)
        throw std::runtime_error("can't allocate wavelet workspace");
    d_temp = (double*)malloc(d_size * sizeof(double));
    if (d_temp == NULL)
        throw std::runtime_error("can't allocate wavelet double conversion temp");
}

wavelet_ff_impl::~wavelet_ff_impl()
{
    gsl_wavelet_free(d_wavelet);
    gsl_wavelet_workspace_free(d_workspace);
    free((char*)d_temp);
}

int wavelet_ff_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    for (int count = 0; count < noutput_items; count++) {
        for (int i = 0; i < d_size; i++)
            d_temp[i] = in[i];

        if (d_forward)
            gsl_wavelet_transform_forward(d_wavelet, d_temp, 1, d_size, d_workspace);
        else
            gsl_wavelet_transform_inverse(d_wavelet, d_temp, 1, d_size, d_workspace);

        for (int i = 0; i < d_size; i++)
            out[i] = d_temp[i];

        in += d_size;
        out += d_size;
    }

    return noutput_items;
}

} /* namespace wavelet */
} /* namespace gr */
