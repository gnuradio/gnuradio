/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FFT_FFT_VFC_H
#define INCLUDED_FFT_FFT_VFC_H

#include <fft/api.h>
#include <gr_sync_block.h>

namespace gr {
  namespace fft {

    class FFT_API fft_vfc : virtual public gr_sync_block
    {
    public:

      // gr::fft::fft_vfc::sptr
      typedef boost::shared_ptr<fft_vfc> sptr;
      
      /*!
       * \brief Compute forward or reverse FFT. float vector in / complex vector out.
       * \ingroup dft_blk
       */
      static FFT_API sptr make(int fft_size, bool forward,
			       const std::vector<float> &window,
			       int nthreads=1);

      virtual void set_nthreads(int n)
      { throw std::runtime_error("fft_vfc::set_nthreads not implemented.\n"); }

      virtual int nthreads() const
      { throw std::runtime_error("fft_vfc::nthreads not implemented.\n"); }

      virtual bool set_window(const std::vector<float> &window)
      { throw std::runtime_error("fft_vfc::set_window not implemented.\n"); }
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_FFT_VFC_H */
