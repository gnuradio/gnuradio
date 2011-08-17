/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

/*
 *   --- ATSC Segment and Symbol Sync Recovery ---
 */

#ifndef _ATSC_SSSR_H_
#define _ATSC_SSSR_H_

#include <atsc_api.h>
#include <atsc_consts.h>
#include <gri_mmse_fir_interpolator.h>
#include <gr_single_pole_iir.h>
#include <cstdio>

/*
 *   --- support classes for atsci_sssr ---
 */

namespace sssr {

  typedef float sample_t;

  // ----------------------------------------------------------------
  //! digital correlator for 1001 and 0110 patterns

  class ATSC_API digital_correlator {
    int		d_sr;	// 4 bit shift register

  public:
    
    // Constructor
    digital_correlator () { reset (); }

    // Manipulators

    //! called on channel change
    void reset () { d_sr = 0; }

    //! clock bit in and return true if we've seen 1001

    bool update (int bit) {
      d_sr = ((bit & 1) << 3) | (d_sr >> 1);

      return (d_sr == 0x9);	// 1001
    }

  };


  // ----------------------------------------------------------------
  //! segment sync integrator

  class ATSC_API seg_sync_integrator {
    signed char	d_integrator[ATSC_DATA_SEGMENT_LENGTH];
    
  public:

    // Constructor
    seg_sync_integrator () { reset (); }

    // Manipulators

    //! called on channel change
    void reset ();
    
    //! update current tap with weight and return integrated correlation value
    int update (int weight, int index);

    //! return index of maximum correlation value
    int find_max (int *value);

  };
  
  // ----------------------------------------------------------------
  //! quad filter (used to compute timing error)

  class ATSC_API quad_filter {
    sample_t	d_delay[4];

  public:
    // Constructor
    quad_filter () { reset (); }

    // Manipulators

    //! called on channel change
    void reset () { d_delay[0] = d_delay[1] = d_delay[2] = d_delay[3] = 0; }

    double update (sample_t sample){
      d_delay[3] = d_delay[2];
      d_delay[2] = d_delay[1];
      d_delay[1] = d_delay[0];
      d_delay[0] = sample;

      // the coefficients are -1,-1,+1,+1
      return d_delay[3] + d_delay[2] - d_delay[1] - d_delay[0];
    }
  };
}

// ----------------------------------------------------------------

/*!
 * \brief ATSC Segment and Symbol Sync Recovery
 *
 * This class implements data segment sync tracking and symbol timing
 * using the method described in "ATSC/VSB Tutorial - Receiver Technology"
 * by Wayne E. Bretl of Zenith, pgs 41-45.
 */

class ATSC_API atsci_sssr {
  sssr::digital_correlator	d_correlator;
  sssr::seg_sync_integrator	d_integrator;
  sssr::quad_filter		d_quad_filter;
  double			d_quad_output[ATSC_DATA_SEGMENT_LENGTH];
  double			d_timing_adjust;
  int				d_counter;	// free running mod 832 counter
  int				d_symbol_index;
  bool				d_seg_locked;
  FILE			       *d_debug_fp;

  
  bool incr_counter () {
    d_counter++;
    if (d_counter >= ATSC_DATA_SEGMENT_LENGTH){
      d_counter = 0;
      return true;
    }
    return false;
  }
    
  void incr_symbol_index () {
    d_symbol_index++;
    if (d_symbol_index >= ATSC_DATA_SEGMENT_LENGTH)
      d_symbol_index = 0;
  }

public:

  // Constructor
  atsci_sssr ();
  ~atsci_sssr ();

  // Manipulators

  //! call on channel change
  void reset ();


  /*!
   * \brief process a single sample at the ATSC symbol rate (~10.76 MSPS)
   *
   * This block computes an indication of our timing error and keeps
   * track of where the segment sync's occur.  \p timing_adjust is
   * returned to indicate how the interpolator timing needs to be
   * adjusted to track the transmitter's symbol timing.  If \p seg_locked
   * is true, then \p symbol_index is the index of this sample in 
   * the current segment.  The symbols are numbered from 0 to 831, where 
   * symbols 0, 1, 2 and 3 correspond to the data segment sync pattern, 
   * nominally +5, -5, -5, +5.
   */

  void update (sssr::sample_t  sample_in,	// input
	       bool	      *seg_locked,	// are we seeing segment syncs?
	       int	      *symbol_index,	// 0..831
	       double	      *timing_adjust);	// how much to adjust timing

};

// ----------------------------------------------------------------

/*!
 * \brief interpolator control for segment and symbol sync recovery
 */
 
class ATSC_API atsci_interpolator {
  gri_mmse_fir_interpolator	d_interp;
  gr_single_pole_iir<float,float,float> d_loop;	// ``VCO'' loop filter
  double			d_nominal_ratio_of_rx_clock_to_symbol_freq; // FREQ
  double			d_w;	// ratio of PERIOD of Tx to Rx clocks
  double			d_mu;	// fractional delay [0,1]
  int				d_incr;		// diagnostic only
  FILE			       *d_debug_fp;	// diagnostic only

public:
  //! \p nominal_ratio_of_rx_clock_to_symbol_freq must be >= 1.8
  atsci_interpolator (double nominal_ratio_of_rx_clock_to_symbol_freq);
  ~atsci_interpolator ();

  // Manipulators

  //! call on channel change
  void reset ();

  /*!
   * \brief produce next sample referenced to Tx clock
   *
   * If there aren't enough input_samples left to produce
   * an output, return false, else true.
   */

  bool update (const sssr::sample_t input_samples[],	// I: vector of samples
	       int	      	    nsamples,		// I: total number of samples
	       int	           *index,		// I/O: current input index
	       double		    timing_adjustment,	// I: how much to bump timing
	       sssr::sample_t	   *output_sample);	// O: the output sample

  // Accessors

  // how much history we require on our input
  unsigned ntaps () const { return d_interp.ntaps (); }

  // diagnostic accessors
  double mu ()   const { return d_mu; }
  double w ()    const { return d_w;  }
  int	 incr () const { return d_incr; }
  
};

#endif /* _ATSC_SSSR_H_ */
