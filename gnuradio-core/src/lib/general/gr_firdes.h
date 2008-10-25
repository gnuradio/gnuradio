/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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

#ifndef _GR_FIRDES_H_
#define _GR_FIRDES_H_

#include <vector>
#include <cmath>
#include <gr_complex.h>

/*!
 * \brief Finite Impulse Response (FIR) filter design functions.
 * \ingroup filter_design
 */

class gr_firdes {
 public:

  enum win_type {
    WIN_HAMMING = 0,	// max attenuation 53 dB
    WIN_HANN = 1,	// max attenuation 44 dB
    WIN_BLACKMAN = 2,	// max attenuation 74 dB
    WIN_RECTANGULAR = 3,
    WIN_KAISER = 4,     // max attenuation a function of beta, google it
    WIN_BLACKMAN_hARRIS = 5,
  };


  // ... class methods ...

  /*!
   * \brief use "window method" to design a low-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */
  static std::vector<float>
  low_pass (double gain,
	    double sampling_freq,
	    double cutoff_freq,		// Hz center of transition band
	    double transition_width,	// Hz width of transition band
	    win_type window = WIN_HAMMING,
	    double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a low-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   * \p attenuation_dB          required stopband attenuation
   *				The normalized width of the transition
   *				band and the required stop band
   *                            attenuation is what sets the number of taps
   *				required.  Narrow --> more taps
   *                            More attenuatin --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  low_pass_2 (double gain,
	    double sampling_freq,
	    double cutoff_freq,		// Hz beginning transition band
	    double transition_width,	// Hz width of transition band
	    double attenuation_dB,      // out of band attenuation dB
	    win_type window = WIN_HAMMING,
	    double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a high-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  high_pass (double gain,
	     double sampling_freq,
	     double cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a high-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p cutoff_freq:		center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   * \p attenuation_dB          out of band attenuation
   *				The normalized width of the transition
   *				band and the required stop band
   *                            attenuation is what sets the number of taps
   *				required.  Narrow --> more taps
   *                            More attenuation --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  high_pass_2 (double gain,
	     double sampling_freq,
	     double cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     double attenuation_dB,             // out of band attenuation dB
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a band-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */
  static std::vector<float>
  band_pass (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz center of transition band
	     double high_cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a band-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   * \p attenuation_dB          out of band attenuation
   *				The normalized width of the transition
   *				band and the required stop band
   *                            attenuation is what sets the number of taps
   *				required.  Narrow --> more taps
   *                            More attenuation --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  band_pass_2 (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz beginning transition band
	     double high_cutoff_freq,		// Hz beginning transition band
	     double transition_width,		// Hz width of transition band
	     double attenuation_dB,             // out of band attenuation dB
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a complex band-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<gr_complex>
  complex_band_pass (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz center of transition band
	     double high_cutoff_freq,		// Hz center of transition band
	     double transition_width,		// Hz width of transition band
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a complex band-pass FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   * \p attenuation_dB          out of band attenuation
   *				The normalized width of the transition
   *				band and the required stop band
   *                            attenuation is what sets the number of taps
   *				required.  Narrow --> more taps
   *                            More attenuation --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<gr_complex>
  complex_band_pass_2 (double gain,
	     double sampling_freq,
	     double low_cutoff_freq,		// Hz beginning transition band
	     double high_cutoff_freq,		// Hz beginning transition band
	     double transition_width,		// Hz width of transition band
	     double attenuation_dB,             // out of band attenuation dB
	     win_type window = WIN_HAMMING,
	     double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a band-reject FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   *				The normalized width of the transition
   *				band is what sets the number of taps
   *				required.  Narrow --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  band_reject (double gain,
	       double sampling_freq,
	       double low_cutoff_freq,		// Hz center of transition band
	       double high_cutoff_freq,		// Hz center of transition band
	       double transition_width,		// Hz width of transition band
	       win_type window = WIN_HAMMING,
	       double beta = 6.76);		// used only with Kaiser

  /*!
   * \brief use "window method" to design a band-reject FIR filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p low_cutoff_freq:	center of transition band (Hz)
   * \p high_cutoff_freq:	center of transition band (Hz)
   * \p transition_width:	width of transition band (Hz).
   * \p attenuation_dB          out of band attenuation
   *				The normalized width of the transition
   *				band and the required stop band
   *                            attenuation is what sets the number of taps
   *				required.  Narrow --> more taps
   *                            More attenuation --> more taps
   * \p window_type: 		What kind of window to use. Determines
   *				maximum attenuation and passband ripple.
   * \p beta:			parameter for Kaiser window
   */

  static std::vector<float>
  band_reject_2 (double gain,
	       double sampling_freq,
	       double low_cutoff_freq,		// Hz beginning transition band
	       double high_cutoff_freq,		// Hz beginning transition band
	       double transition_width,		// Hz width of transition band
	       double attenuation_dB,           // out of band attenuation dB
	       win_type window = WIN_HAMMING,
	       double beta = 6.76);		// used only with Kaiser

  /*!\brief design a Hilbert Transform Filter
   *
   * \p ntaps:                  Number of taps, must be odd
   * \p window_type:            What kind of window to use
   * \p beta:                   Only used for Kaiser
   */
  static std::vector<float>
  hilbert (unsigned int ntaps = 19,
	   win_type windowtype = WIN_RECTANGULAR,
	   double beta = 6.76);
   
  /*!
   * \brief design a Root Cosine FIR Filter (do we need a window?)
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p sampling_freq:		sampling freq (Hz)
   * \p symbol rate:		symbol rate, must be a factor of sample rate
   * \p alpha:		        excess bandwidth factor
   * \p ntaps:		        number of taps
   */
  static std::vector<float>
  root_raised_cosine (double gain,
		      double sampling_freq,
		      double symbol_rate,       // Symbol rate, NOT bitrate (unless BPSK)
		      double alpha,             // Excess Bandwidth Factor
		      int ntaps);

  /*!
   * \brief design a Gaussian filter
   *
   * \p gain:			overall gain of filter (typically 1.0)
   * \p symbols per bit:	symbol rate, must be a factor of sample rate
   * \p ntaps:		        number of taps
   */
  static std::vector<float>
  gaussian (double gain,
	    double spb,       
	    double bt,              // Bandwidth to bitrate ratio
	    int ntaps);

  // window functions ...
  static std::vector<float> window (win_type type, int ntaps, double beta);

private:
  static double bessi0(double x);
  static void sanity_check_1f (double sampling_freq, double f1,
			       double transition_width);
  static void sanity_check_2f (double sampling_freq, double f1, double f2,
			       double transition_width);
  static void sanity_check_2f_c (double sampling_freq, double f1, double f2,
			       double transition_width);

  static int compute_ntaps (double sampling_freq,
			    double transition_width,
			    win_type window_type, double beta);

  static int compute_ntaps_windes (double sampling_freq,
				   double transition_width,
				   double attenuation_dB);
};

#endif
