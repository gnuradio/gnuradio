/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_WAVELET_WVPS_FF_H
#define INCLUDED_WAVELET_WVPS_FF_H

#include <gnuradio/wavelet/api.h>
#include <gnuradio/sync_decimator.h>

class wavelet_wvps_ff;

namespace gr {
  namespace wavelet {

    /*!
     * \brief computes the Wavelet Power Spectrum from a set of wavelet coefficients
     * \ingroup wavelet_blk
     */
    class WAVELET_API wvps_ff : virtual public sync_block
    {
    public:

      // gr::wavelet::wvps_ff::sptr
      typedef boost::shared_ptr<wvps_ff> sptr;

      /*!
        \param ilen
       */
      static sptr make(int ilen);
    };

  } /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WVPS_FF_H */
