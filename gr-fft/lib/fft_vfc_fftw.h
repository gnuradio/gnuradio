/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H
#define INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/fft_vfc.h>

namespace gr {
namespace fft {

class FFT_API fft_vfc_fftw : public fft_vfc
{
private:
    const unsigned int d_fft_size;
    fft_complex d_fft;
    std::vector<float> d_window;

public:
    fft_vfc_fftw(int fft_size,
                 bool forward,
                 const std::vector<float>& window,
                 int nthreads = 1);

    void set_nthreads(int n);
    int nthreads() const;
    bool set_window(const std::vector<float>& window);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_FFT_VFC_FFTW_IMPL_H */
