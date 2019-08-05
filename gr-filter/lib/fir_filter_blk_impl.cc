/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fir_filter_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
typename fir_filter_blk<IN_T, OUT_T, TAP_T>::sptr
fir_filter_blk<IN_T, OUT_T, TAP_T>::make(int decimation, const std::vector<TAP_T>& taps)
{
    return gnuradio::get_initial_sptr(
        new fir_filter_blk_impl<IN_T, OUT_T, TAP_T>(decimation, taps));
}


template <class IN_T, class OUT_T, class TAP_T>
fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::fir_filter_blk_impl(
    int decimation, const std::vector<TAP_T>& taps)
    : sync_decimator("fir_filter_blk<IN_T,OUT_T,TAP_T>",
                     io_signature::make(1, 1, sizeof(IN_T)),
                     io_signature::make(1, 1, sizeof(OUT_T)),
                     decimation)
{
    d_fir = new kernel::fir_filter<IN_T, OUT_T, TAP_T>(decimation, taps);
    d_updated = false;
    this->set_history(d_fir->ntaps());

    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    this->set_alignment(std::max(1, alignment_multiple));
}

template <class IN_T, class OUT_T, class TAP_T>
fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::~fir_filter_blk_impl()
{
    delete d_fir;
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_fir->set_taps(taps);
    d_updated = true;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_fir->taps();
}

template <class IN_T, class OUT_T, class TAP_T>
int fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::work(int noutput_items,
                                                  gr_vector_const_void_star& input_items,
                                                  gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];

    if (d_updated) {
        this->set_history(d_fir->ntaps());
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    if (this->decimation() == 1) {
        d_fir->filterN(out, in, noutput_items);
    } else {
        d_fir->filterNdec(out, in, noutput_items, this->decimation());
    }

    return noutput_items;
}
template class fir_filter_blk<gr_complex, gr_complex, gr_complex>;
template class fir_filter_blk<gr_complex, gr_complex, float>;
template class fir_filter_blk<float, gr_complex, gr_complex>;
template class fir_filter_blk<float, float, float>;
template class fir_filter_blk<float, std::int16_t, float>;
template class fir_filter_blk<std::int16_t, gr_complex, gr_complex>;
} /* namespace filter */
} /* namespace gr */
