/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
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


#ifndef INCLUDED_GR_PFB_SYNTHESIZER_CCF_H
#define	INCLUDED_GR_PFB_SYNTHESIZER_CCF_H

#include <gr_core_api.h>
#include <gr_sync_interpolator.h>
#include <gri_fir_filter_with_buffer_ccf.h>
#include <gruel/thread.h>

class gr_pfb_synthesizer_ccf;
typedef boost::shared_ptr<gr_pfb_synthesizer_ccf> gr_pfb_synthesizer_ccf_sptr;
GR_CORE_API gr_pfb_synthesizer_ccf_sptr gr_make_pfb_synthesizer_ccf
    (unsigned int numchans, const std::vector<float> &taps, bool twox=false);

class gri_fft_complex;


/*!
 * \class gr_pfb_synthesizer_ccf
 *
 * \brief Polyphase synthesis filterbank with
 *        gr_complex input, gr_complex output and float taps
 *
 * \ingroup filter_blk
 * \ingroup pfb_blk
 */

class GR_CORE_API gr_pfb_synthesizer_ccf : public gr_sync_interpolator
{
 private:
  /*!
   * Build the polyphase synthesis filterbank.
   * \param numchans (unsigned integer) Specifies the number of
                     channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter to
                    populate the filterbank.
   * \param twox    (bool) use 2x oversampling or not (default is no)
   */
  friend GR_CORE_API gr_pfb_synthesizer_ccf_sptr gr_make_pfb_synthesizer_ccf
    (unsigned int numchans, const std::vector<float> &taps, bool twox);

  bool			   d_updated;
  unsigned int             d_numchans;
  unsigned int             d_taps_per_filter;
  gri_fft_complex         *d_fft;
  std::vector< gri_fir_filter_with_buffer_ccf*> d_filters;
  std::vector< std::vector<float> > d_taps;
  int              d_state;
  std::vector<int> d_channel_map;
  unsigned int     d_twox;
  gruel::mutex     d_mutex; // mutex to protect set/work access

  /*!
   * \brief Tap setting algorithm for critically sampled channels
   */
  void set_taps1(const std::vector<float> &taps);

  /*!
   * \brief Tap setting algorithm for 2x over-sampled channels
   */
  void set_taps2(const std::vector<float> &taps);

  /*!
   * Build the polyphase synthesis filterbank.
   * \param numchans (unsigned integer) Specifies the number of
                     channels <EM>M</EM>
   * \param taps    (vector/list of floats) The prototype filter
                    to populate the filterbank.
   * \param twox    (bool) use 2x oversampling or not (default is no)
   */
  gr_pfb_synthesizer_ccf (unsigned int numchans,
			  const std::vector<float> &taps,
			  bool twox);

public:
  ~gr_pfb_synthesizer_ccf ();

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

  /*!
   * Return a vector<vector<>> of the filterbank taps
   */
  std::vector<std::vector<float> > taps() const;

  /*!
   * Set the channel map. Channels are numbers as:
   *     N/2+1 | ... | N-1 | 0 | 1 |  2 | ... | N/2
   *    <------------------- 0 -------------------->
   *                        freq
   *
   * So input stream 0 goes to channel 0, etc. Setting a new channel
   * map allows the user to specify where in frequency he/she wants
   * the input stream to go. This is especially useful to avoid
   * putting signals into the channels on the edge of the spectrum
   * which can either wrap around (in the case of odd number of
   * channels) and be affected by filter rolloff in the transmitter.
   *
   * The map must be at least the number of streams being sent to the
   * block. Less and the algorithm will not have enough data to
   * properly setup the buffers. Any more channels specified will be
   * ignored.
   */
  void set_channel_map(const std::vector<int> &map);

  /*!
   * Gets the current channel map.
   */
  std::vector<int> channel_map() const;

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
