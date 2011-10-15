/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GR_PFB_SYNTHESIS_FILTERBANK_CCF_H
#define	INCLUDED_GR_PFB_SYNTHESIS_FILTERBANK_CCF_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>
#include <gri_fir_filter_with_buffer_ccf.h>

class gr_pfb_synthesis_filterbank_ccf;
typedef boost::shared_ptr<gr_pfb_synthesis_filterbank_ccf> gr_pfb_synthesis_filterbank_ccf_sptr;
GR_CORE_API gr_pfb_synthesis_filterbank_ccf_sptr gr_make_pfb_synthesis_filterbank_ccf 
    (unsigned int numchans, const std::vector<float> &taps);

class gri_fft_complex;


/*!
 * \class gr_pfb_synthesis_filterbank_ccf
 *
 * \brief Polyphase synthesis filterbank with 
 *        gr_complex input, gr_complex output and float taps
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 */

class GR_CORE_API gr_pfb_synthesis_filterbank_ccf : public gr_sync_interpolator
{
 private:
  /*!
   * Build the polyphase synthesis filterbank.
   * \param numchans (unsigned integer) Specifies the number of 
                     channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter to
                    populate the filterbank.
   */
  friend GR_CORE_API gr_pfb_synthesis_filterbank_ccf_sptr gr_make_pfb_synthesis_filterbank_ccf 
      (unsigned int numchans, const std::vector<float> &taps);

  bool			   d_updated;
  unsigned int             d_numchans;
  unsigned int             d_taps_per_filter;
  gri_fft_complex         *d_fft;
  std::vector< gri_fir_filter_with_buffer_ccf*> d_filters;
  std::vector< std::vector<float> > d_taps;


  /*!
   * Build the polyphase synthesis filterbank.
   * \param numchans (unsigned integer) Specifies the number of
                     channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter
                    to populate the filterbank.
   */
  gr_pfb_synthesis_filterbank_ccf (unsigned int numchans, 
				   const std::vector<float> &taps);
  
public:
  ~gr_pfb_synthesis_filterbank_ccf ();
  
  /*!
   * Resets the filterbank's filter taps with the new prototype filter
   * \param taps    (vector/list of floats) The prototype filter to
                    populate the filterbank.
   */
  void set_taps (const std::vector<float> &taps);

  /*!
   * Print all of the filterbank taps to screen.
   */
  void print_taps();
  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
