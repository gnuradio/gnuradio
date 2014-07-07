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

#ifndef INCLUDED_WAVELET_WAVELET_FF_H
#define INCLUDED_WAVELET_WAVELET_FF_H

#include <gnuradio/wavelet/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace wavelet {

    /*!
     * \brief Compute wavelet transform using gsl routines.
     * \ingroup wavelet_blk
     */
    class WAVELET_API wavelet_ff : virtual public sync_block
    {
    public:

      // gr::wavelet::wavelet_ff:sptr
      typedef boost::shared_ptr<wavelet_ff> sptr;

      /*!
       * \param size
       * \param order
       * \param forward
       */
      static sptr make(int size = 1024,
                       int order = 20,
                       bool forward = true);
    };

  } /* namespace wavelet */
} /* namespace gr */

#endif /* INCLUDED_WAVELET_WAVELET_FF_H */
