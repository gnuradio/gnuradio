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

#ifndef	INCLUDED_WAVELET_SQUASH_FF_H
#define	INCLUDED_WAVELET_SQUASH_FF_H

#include <gnuradio/wavelet/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace wavelet {

    /*!
     * \brief Implements cheap resampling of spectrum directly from
     * spectral points, using gsl interpolation
     * \ingroup misc
     */
    class WAVELET_API squash_ff : virtual public sync_block
    {
    public:

      // gr::wavelet::squash_ff::sptr
      typedef boost::shared_ptr<squash_ff> sptr;

      /*!
       * \param igrid
       * \param ogrid
       */
      static sptr make(const std::vector<float> &igrid,
                       const std::vector<float> &ogrid);
    };

  } /* namespace wavelet */
} /* namespace gr */

#endif
