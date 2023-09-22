/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "iir_filter_ccf_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace filter {

iir_filter_ccf::sptr iir_filter_ccf::make(const std::vector<float>& fftaps,
                                          const std::vector<float>& fbtaps,
                                          bool oldstyle)
{
    return gnuradio::make_block_sptr<iir_filter_ccf_impl>(fftaps, fbtaps, oldstyle);
}

iir_filter_ccf_impl::iir_filter_ccf_impl(const std::vector<float>& fftaps,
                                         const std::vector<float>& fbtaps,
                                         bool oldstyle)

    : sync_block("iir_filter_ccf",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_updated(false),
      d_iir(fftaps, fbtaps, oldstyle)
{
}

void iir_filter_ccf_impl::set_taps(const std::vector<float>& fftaps,
                                   const std::vector<float>& fbtaps)
{
    d_new_fftaps = fftaps;
    d_new_fbtaps = fbtaps;
    d_updated = true;
}

int iir_filter_ccf_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    if (d_updated) {
        d_iir.set_taps(d_new_fftaps, d_new_fbtaps);
        d_updated = false;
    }

    d_iir.filter_n(out, in, noutput_items);
    return noutput_items;
};

} /* namespace filter */
} /* namespace gr */
