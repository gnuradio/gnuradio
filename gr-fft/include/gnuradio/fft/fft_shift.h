/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FFT_FFT_SHIFT_H
#define INCLUDED_FFT_FFT_SHIFT_H

#include <algorithm>
#include <cassert>
#include <vector>

namespace gr {
namespace fft {

/*! \brief reorder FFT results which are ordered from 0 to 1 in normalized frequency
 *  to -0.5 to 0.5 by cyclic shift
 */
template <typename T>
class fft_shift
{
public:
    fft_shift(size_t fft_length)
        : d_fftlen(fft_length),
          d_lenpos(fft_length / 2 + (fft_length % 2)),
          d_lenneg(fft_length / 2),
          d_buf(fft_length)
    {
    }

    /*! performs the cyclic shift on a vector v
     */
    void shift(std::vector<T>& v) { shift(&v.front(), v.size()); }

    /*! performs the cyclic shift on an array
     */
    void shift(T* data, size_t fft_len)
    {
        resize(fft_len);
        std::copy_n(data, d_lenpos, d_buf.begin());
        std::copy_n(data + d_lenpos, d_lenneg, data);
        std::copy_n(d_buf.begin(), d_lenpos, data + d_lenneg);
    }

    /*! if needed adjusts the buffer size to a new fft length
     */
    void resize(size_t fft_length)
    {
        if (d_fftlen == fft_length)
            return;
        d_fftlen = fft_length;
        d_lenpos = d_fftlen / 2 + (d_fftlen % 2);
        d_lenneg = d_fftlen / 2;
        assert(d_lenpos + d_lenneg == d_fftlen);
        d_buf.resize(d_lenpos);
    }

protected:
private:
    size_t d_fftlen;      // FFT length
    size_t d_lenpos;      // number of FFT bins with positive frequencies
    size_t d_lenneg;      // number of FFT bins with negative frequencies
    std::vector<T> d_buf; // buffer used for cyclic shift
};

} // namespace fft
} // namespace gr
#endif // INCLUDED_FFT_FFT_SHIFT_H
