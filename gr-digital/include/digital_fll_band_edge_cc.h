/* -*- c++ -*- */
/*
 * Copyright 2009,2011 Free Software Foundation, Inc.
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


#ifndef INCLUDED_DIGITAL_FLL_BAND_EDGE_CC_H
#define	INCLUDED_DIGITAL_FLL_BAND_EDGE_CC_H

#include <digital_api.h>
#include <gr_sync_block.h>
#include <gri_control_loop.h>

class digital_fll_band_edge_cc;
typedef boost::shared_ptr<digital_fll_band_edge_cc> digital_fll_band_edge_cc_sptr;
DIGITAL_API digital_fll_band_edge_cc_sptr digital_make_fll_band_edge_cc (float samps_per_sym,
							     float rolloff,
							     int filter_size,
							     float bandwidth);

/*!
 * \class digital_fll_band_edge_cc
 * \brief Frequency Lock Loop using band-edge filters
 *
 * \ingroup general
 * \ingroup digital
 *
 * The frequency lock loop derives a band-edge filter that covers the
 * upper and lower bandwidths of a digitally-modulated signal. The
 * bandwidth range is determined by the excess bandwidth (e.g.,
 * rolloff factor) of the modulated signal. The placement in frequency
 * of the band-edges is determined by the oversampling ratio (number
 * of samples per symbol) and the excess bandwidth.  The size of the
 * filters should be fairly large so as to average over a number of
 * symbols.
 *
 * The FLL works by filtering the upper and lower band edges into
 * x_u(t) and x_l(t), respectively.  These are combined to form cc(t)
 * = x_u(t) + x_l(t) and ss(t) = x_u(t) - x_l(t). Combining these to
 * form the signal e(t) = Re{cc(t) \\times ss(t)^*} (where ^* is the
 * complex conjugate) provides an error signal at the DC term that is
 * directly proportional to the carrier frequency.  We then make a
 * second-order loop using the error signal that is the running
 * average of e(t).
 *
 * In practice, the above equation can be simplified by just comparing
 * the absolute value squared of the output of both filters:
 * abs(x_l(t))^2 - abs(x_u(t))^2 = norm(x_l(t)) - norm(x_u(t)).
 *
 * In theory, the band-edge filter is the derivative of the matched
 * filter in frequency, (H_be(f) = \\frac{H(f)}{df}. In practice, this
 * comes down to a quarter sine wave at the point of the matched
 * filter's rolloff (if it's a raised-cosine, the derivative of a
 * cosine is a sine).  Extend this sine by another quarter wave to
 * make a half wave around the band-edges is equivalent in time to the
 * sum of two sinc functions. The baseband filter fot the band edges
 * is therefore derived from this sum of sincs. The band edge filters
 * are then just the baseband signal modulated to the correct place in
 * frequency. All of these calculations are done in the
 * 'design_filter' function.
 *
 * Note: We use FIR filters here because the filters have to have a
 * flat phase response over the entire frequency range to allow their
 * comparisons to be valid.
 *
 * It is very important that the band edge filters be the derivatives
 * of the pulse shaping filter, and that they be linear
 * phase. Otherwise, the variance of the error will be very large.
 *
 */

class DIGITAL_API digital_fll_band_edge_cc : public gr_sync_block, public gri_control_loop
{
 private:
  /*!
   * Build the FLL
   * \param samps_per_sym    (float) Number of samples per symbol of signal
   * \param rolloff          (float) Rolloff factor of signal
   * \param filter_size      (int)   Size (in taps) of the filter
   * \param bandwidth        (float) Loop bandwidth
   */
  friend DIGITAL_API digital_fll_band_edge_cc_sptr digital_make_fll_band_edge_cc (float samps_per_sym,
								      float rolloff,
								      int filter_size,
								      float bandwidth);

  float                   d_sps;
  float                   d_rolloff;
  int                     d_filter_size;

  std::vector<gr_complex> d_taps_lower;
  std::vector<gr_complex> d_taps_upper;
  bool			  d_updated;

  /*!
   * Build the FLL
   * \param samps_per_sym (float) number of samples per symbol
   * \param rolloff (float) Rolloff (excess bandwidth) of signal filter
   * \param filter_size (int) number of filter taps to generate
   * \param bandwidth (float) Loop bandwidth
   */
  digital_fll_band_edge_cc(float samps_per_sym, float rolloff,
			   int filter_size, float bandwidth);
  
  /*!
   * Design the band-edge filter based on the number of samples per symbol,
   * filter rolloff factor, and the filter size
   *
   * \param samps_per_sym    (float) Number of samples per symbol of signal
   * \param rolloff          (float) Rolloff factor of signal
   * \param filter_size      (int)   Size (in taps) of the filter
   */
  void design_filter(float samps_per_sym, float rolloff, int filter_size);

public:
  ~digital_fll_band_edge_cc ();

  /*******************************************************************
    SET FUNCTIONS
  *******************************************************************/
  
  /*!
   * \brief Set the number of samples per symbol
   *
   * Set's the number of samples per symbol the system should
   * use. This value is uesd to calculate the filter taps and will
   * force a recalculation.
   *
   * \param sps    (float) new samples per symbol
   *
   */
  void set_samples_per_symbol(float sps);

  /*!
   * \brief Set the rolloff factor of the shaping filter
   *
   * This sets the rolloff factor that is used in the pulse shaping
   * filter and is used to calculate the filter taps. Changing this
   * will force a recalculation of the filter taps.
   *
   * This should be the same value that is used in the transmitter's
   * pulse shaping filter. It must be between 0 and 1 and is usually
   * between 0.2 and 0.5 (where 0.22 and 0.35 are commonly used
   * values).
   *
   * \param rolloff    (float) new shaping filter rolloff factor [0,1]
   *
   */
  void set_rolloff(float rolloff);

  /*!
   * \brief Set the number of taps in the filter
   *
   * This sets the number of taps in the band-edge filters. Setting
   * this will force a recalculation of the filter taps.
   *
   * This should be about the same number of taps used in the
   * transmitter's shaping filter and also not very large. A large
   * number of taps will result in a large delay between input and
   * frequency estimation, and so will not be as accurate. Between 30
   * and 70 taps is usual.
   *
   * \param filter_size    (float) number of taps in the filters
   *
   */
  void set_filter_size(int filter_size);

  /*******************************************************************
    GET FUNCTIONS
  *******************************************************************/

  /*!
   * \brief Returns the number of sampler per symbol used for the filter
   */
  float get_samples_per_symbol() const;

  /*!
   * \brief Returns the rolloff factor used for the filter
   */
  float get_rolloff() const;

  /*!
   * \brief Returns the number of taps of the filter
   */
  int get_filter_size() const;

  /*!
   * Print the taps to screen.
   */
  void print_taps();
   
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif
