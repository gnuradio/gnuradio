/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2008,2012,2020 Free Software Foundation, Inc.
 * Copyright 2021 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/fft/fft.h>
#include <gnuradio/kernel/fft/fftw_fft.h>

namespace gr {
namespace fft {

template <class T, bool forward>
class fft_cpu : public fft<T, forward>
{
public:
    fft_cpu(const typename fft<T, forward>::block_args& args);
    work_return_code_t work(work_io&) override;

    void set_nthreads(int n);
    int nthreads() const;

protected:
    size_t d_fft_size;
    std::vector<float> d_window;
    bool d_shift;

    kernel::fft::fftw_fft<gr_complex, forward> d_fft;

    void fft_and_shift(const T* in, gr_complex* out);
};

} // namespace fft
} // namespace gr