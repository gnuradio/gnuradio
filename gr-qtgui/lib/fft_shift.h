/* -*- c++ -*- */
/*
 * Copyright 2012,2014-2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_FFT_SHIFT_H
#define INCLUDED_QTGUI_FFT_SHIFT_H

#include <algorithm>
#include <vector>

namespace gr {
  namespace qtgui {

    class fft_shift {
    public:
      fft_shift(size_t fft_length)
	: d_fftlen()
	, d_lenpos()
	, d_lenneg()
	, d_buf() {
	resize(fft_length);
      }

      void resize(size_t fft_length) {
	if (d_fftlen == fft_length)
	  return;
	d_fftlen = fft_length;
	d_lenpos = d_fftlen/2 + (d_fftlen%2);
	d_lenneg = d_fftlen/2;
	assert(d_lenpos + d_lenneg == d_fftlen);
	d_buf.resize(d_lenpos);
      }
      void shift(float* data, size_t fft_len) {
	resize(fft_len);
	std::copy_n(data,          d_lenpos, d_buf.begin());
	std::copy_n(data+d_lenpos, d_lenneg, data);
	std::copy_n(d_buf.begin(), d_lenpos, data+d_lenneg);
      }
    protected:
    private:
      size_t d_fftlen;
      size_t d_lenpos;
      size_t d_lenneg;
      std::vector<float> d_buf;
    } ;

  } // namespace qtgui
} // namespace gr
#endif // INCLUDED_QTGUI_FFT_SHIFT_H
