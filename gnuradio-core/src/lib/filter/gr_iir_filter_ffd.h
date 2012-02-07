/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_IIR_FILTER_FFD_H
#define	INCLUDED_GR_IIR_FILTER_FFD_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_iir.h>
#include <stdexcept>

class gr_iir_filter_ffd;
typedef boost::shared_ptr<gr_iir_filter_ffd> gr_iir_filter_ffd_sptr;
GR_CORE_API gr_iir_filter_ffd_sptr 
gr_make_iir_filter_ffd (const std::vector<double> &fftaps,
			const std::vector<double> &fbtaps) throw (std::invalid_argument);

/*!
 * \brief  IIR filter with float input, float output and double taps
 * \ingroup filter_blk
 *
 * This filter uses the Direct Form I implementation, where
 * \p fftaps contains the feed-forward taps, and \p fbtaps the feedback ones.
 *
 * 
 * The input and output satisfy a difference equation of the form
 \htmlonly
 \f{
 y[n] - \sum_{k=1}^{M} a_k y[n-k] = \sum_{k=0}^{N} b_k x[n-k]
 \f}
 \endhtmlonly

 \xmlonly
 y[n] - \sum_{k=1}^{M} a_k y[n-k] = \sum_{k=0}^{N} b_k x[n-k]
 \endxmlonly

 * with the corresponding rational system function
 \htmlonly
 \f{
 H(z) = \ frac{\sum_{k=0}^{M} b_k z^{-k}}{1 - \sum_{k=1}^{N} a_k z^{-k}}
 \f}
 \endhtmlonly

 \xmlonly
 H(z) = \ frac{\sum_{k=0}^{M} b_k z^{-k}}{1 - \sum_{k=1}^{N} a_k z^{-k}}
 \endxmlonly

 * Note that some texts define the system function with a + in the denominator.
 * If you're using that convention, you'll need to negate the feedback taps.
 */
class GR_CORE_API gr_iir_filter_ffd : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_iir_filter_ffd_sptr 
  gr_make_iir_filter_ffd (const std::vector<double> &fftaps,
			  const std::vector<double> &fbtaps) throw (std::invalid_argument);

  gri_iir<float,float,double>		d_iir;
  std::vector<double>			d_new_fftaps;
  std::vector<double>			d_new_fbtaps;
  bool					d_updated;

  /*!
   * Construct an IIR filter with the given taps
   */
  gr_iir_filter_ffd (const std::vector<double> &fftaps,
		     const std::vector<double> &fbtaps) throw (std::invalid_argument);

 public:
  ~gr_iir_filter_ffd ();

  void set_taps (const std::vector<double> &fftaps,
		 const std::vector<double> &fbtaps) throw (std::invalid_argument);

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
