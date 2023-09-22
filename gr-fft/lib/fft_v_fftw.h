/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_FFT_V_FFTW_IMPL_H
#define INCLUDED_FFT_FFT_V_FFTW_IMPL_H

#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/fft_v.h>

namespace gr {
namespace fft {

template <class T, bool forward>
class FFT_API fft_v_fftw : public fft_v<T, forward>
{
private:
    const unsigned int d_fft_size;
    fft<gr_complex, forward> d_fft;
    std::vector<float> d_window;
    const bool d_shift;
    void fft_and_shift(const T* in, gr_complex* out);

public:
    fft_v_fftw(int fft_size,
               const std::vector<float>& window,
               bool shift,
               int nthreads = 1);

    ~fft_v_fftw() override {}

    void set_nthreads(int n) override;
    int nthreads() const override;
    bool set_window(const std::vector<float>& window) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_FFT_V_FFTW_IMPL_H */
