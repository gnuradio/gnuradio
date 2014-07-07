/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_IIR_FILTER_CCF_H
#define	INCLUDED_IIR_FILTER_CCF_H

#include <gnuradio/filter/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace filter {

    /*!
     * \brief IIR filter with complex input, complex output, and
     * float taps.
     * \ingroup filter_blk
     *
     * \details
     * This filter uses the Direct Form I implementation, where \p
     * fftaps contains the feed-forward taps, and \p fbtaps the
     * feedback ones.
     *
     * \p oldstyle: The old style of the IIR filter uses feedback
     * taps that are negative of what most definitions use (scipy
     * and Matlab among them). This parameter keeps using the old
     * GNU Radio style and is set to TRUE by default. When taps
     * generated from scipy, Matlab, or gr_filter_design, use the
     * new style by setting this to FALSE.
     *
     * The input and output satisfy a difference equation of the form
     \f[
       y[n] - \sum_{k=1}^{M} a_k y[n-k] = \sum_{k=0}^{N} b_k x[n-k]
     \f]

     \xmlonly
     y[n] - \sum_{k=1}^{M} a_k y[n-k] = \sum_{k=0}^{N} b_k x[n-k]
     \endxmlonly

     * with the corresponding rational system function
     \f[
     H(z) = \frac{\sum_{k=0}^{M} b_k z^{-k}}{1 - \sum_{k=1}^{N} a_k z^{-k}}
     \f]

     \xmlonly
     H(z) = \ frac{\sum_{k=0}^{M} b_k z^{-k}}{1 - \sum_{k=1}^{N} a_k z^{-k}}
     \endxmlonly

     */
    class FILTER_API iir_filter_ccf : virtual public sync_block
    {
    public:
      // gr::filter::iir_filter_ccf::sptr
      typedef boost::shared_ptr<iir_filter_ccf> sptr;

      static sptr make(const std::vector<float> &fftaps,
                       const std::vector<float> &fbtaps,
                       bool oldstyle=true);

      virtual void set_taps(const std::vector<float> &fftaps,
			    const std::vector<float> &fbtaps) = 0;
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_IIR_FILTER_CCF_H */
