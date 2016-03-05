/* -*- c++ -*- */
/*
 * Copyright 2002,2008,2012 Free Software Foundation, Inc.
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

#ifndef _FILTER_FIRDES_H_
#define _FILTER_FIRDES_H_

#include <gnuradio/filter/api.h>
#include <vector>
#include <cmath>
#include <gnuradio/gr_complex.h>
#include <gnuradio/fft/window.h>

namespace gr {
  namespace filter {

    /*!
     * \brief Finite Impulse Response (FIR) filter design functions.
     * \ingroup filter_design
     */

    class FILTER_API firdes {
    public:

      // WARNING: deprecated, now located in gr::fft::window.
      // We will be removing this in 3.8.
      enum win_type {
	WIN_NONE = -1,           //!< don't use a window
	WIN_HAMMING = 0,         //!< Hamming window; max attenuation 53 dB
	WIN_HANN = 1,            //!< Hann window; max attenuation 44 dB
	WIN_BLACKMAN = 2,        //!< Blackman window; max attenuation 74 dB
	WIN_RECTANGULAR = 3,     //!< Basic rectangular window
	WIN_KAISER = 4,          //!< Kaiser window; max attenuation a function of beta, google it
	WIN_BLACKMAN_hARRIS = 5, //!< Blackman-harris window
	WIN_BLACKMAN_HARRIS = 5, //!< alias to WIN_BLACKMAN_hARRIS for capitalization consistency
        WIN_BARTLETT = 6,        //!< Barlett (triangular) window
        WIN_FLATTOP = 7,         //!< flat top window; useful in FFTs
      };

      static std::vector<float> window(win_type type, int ntaps, double beta);

      // ... class methods ...

      /*!
       * \brief Use "window method" to design a low-pass FIR filter.  The
       * normalized width of the transition band is what sets the number of
       * taps required.  Narrow --> more taps.  Window type determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param cutoff_freq	        center of transition band (Hz)
       * \param transition_width	width of transition band (Hz)
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      low_pass(double gain,
        double sampling_freq,
        double cutoff_freq,		// Hz center of transition band
        double transition_width,	// Hz width of transition band
        win_type window = WIN_HAMMING,
        double beta = 6.76);		// used only with Kaiser

      /*!
       * \brief Use "window method" to design a low-pass FIR filter.  The
       * normalized width of the transition band and the required stop band
       * attenuation is what sets the number of taps required.  Narrow --> more
       * taps More attenuation --> more taps. The window type determines
       * maximum attentuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param cutoff_freq         beginning of transition band (Hz)
       * \param transition_width    width of transition band (Hz)
       * \param attenuation_dB      required stopband attenuation
       * \param window              one of firdes::win_type
       * \param beta		        parameter for Kaiser window
       */
      static std::vector<float>
      low_pass_2(double gain,
        double sampling_freq,
        double cutoff_freq,		// Hz beginning transition band
        double transition_width,	// Hz width of transition band
        double attenuation_dB,       // out of band attenuation dB
        win_type window = WIN_HAMMING,
        double beta = 6.76);		// used only with Kaiser

      /*!
       * \brief Use "window method" to design a high-pass FIR filter.  The
       * normalized width of the transition band is what sets the number of
       * taps required.  Narrow --> more taps. The window determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param cutoff_freq         center of transition band (Hz)
       * \param transition_width    width of transition band (Hz)
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      high_pass(double gain,
        double sampling_freq,
        double cutoff_freq,		// Hz center of transition band
        double transition_width,	// Hz width of transition band
        win_type window = WIN_HAMMING,
        double beta = 6.76);		// used only with Kaiser

      /*!
       * \brief Use "window method" to design a high-pass FIR filter. The
       * normalized width of the transition band and the required stop band
       * attenuation is what sets the number of taps required.  Narrow --> more
       * taps More attenuation --> more taps. The window determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param cutoff_freq         center of transition band (Hz)
       * \param transition_width    width of transition band (Hz).
       * \param attenuation_dB      out of band attenuation
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      high_pass_2(double gain,
        double sampling_freq,
        double cutoff_freq,		// Hz center of transition band
        double transition_width,	// Hz width of transition band
        double attenuation_dB,      // out of band attenuation dB
        win_type window = WIN_HAMMING,
        double beta = 6.76);	// used only with Kaiser

      /*!
       * \brief Use "window method" to design a band-pass FIR filter. The
       * normalized width of the transition band is what sets the number of
       * taps required.  Narrow --> more taps. The window determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz).
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      band_pass(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz center of transition band
        double high_cutoff_freq,	// Hz center of transition band
        double transition_width,	// Hz width of transition band
        win_type window = WIN_HAMMING,
        double beta = 6.76);		// used only with Kaiser

      /*!
       * \brief Use "window method" to design a band-pass FIR filter.  The
       * normalized width of the transition band and the required stop band
       * attenuation is what sets the number of taps required.  Narrow --> more
       * taps.  More attenuation --> more taps.  Window type determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz).
       * \param attenuation_dB      out of band attenuation
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      band_pass_2(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz beginning transition band
        double high_cutoff_freq,	// Hz beginning transition band
        double transition_width,	// Hz width of transition band
        double attenuation_dB,      // out of band attenuation dB
        win_type window = WIN_HAMMING,
        double beta = 6.76);	// used only with Kaiser

      /*!
       * \brief Use the "window method" to design a complex band-pass FIR
       * filter.  The normalized width of the transition band is what sets the
       * number of taps required.  Narrow --> more taps. The window type
       * determines maximum attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz)
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<gr_complex>
      complex_band_pass(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz center of transition band
        double high_cutoff_freq,	// Hz center of transition band
        double transition_width,	// Hz width of transition band
        win_type window = WIN_HAMMING,
        double beta = 6.76);		// used only with Kaiser

      /*!
       * \brief Use "window method" to design a complex band-pass FIR filter.
       * The normalized width of the transition band and the required stop band
       * attenuation is what sets the number of taps required.  Narrow --> more
       * taps More attenuation --> more taps. Window type determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz)
       * \param attenuation_dB      out of band attenuation
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<gr_complex>
      complex_band_pass_2(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz beginning transition band
        double high_cutoff_freq,	// Hz beginning transition band
        double transition_width,	// Hz width of transition band
        double attenuation_dB,      // out of band attenuation dB
        win_type window = WIN_HAMMING,
        double beta = 6.76);	// used only with Kaiser

      /*!
       * \brief Use "window method" to design a band-reject FIR filter.  The
       * normalized width of the transition band is what sets the number of
       * taps required.  Narrow --> more taps. Window type determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz)
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      band_reject(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz center of transition band
        double high_cutoff_freq,	// Hz center of transition band
        double transition_width,	// Hz width of transition band
        win_type window = WIN_HAMMING,
        double beta = 6.76);	// used only with Kaiser

      /*!
       * \brief Use "window method" to design a band-reject FIR filter.  The
       * normalized width of the transition band and the required stop band
       * attenuation is what sets the number of taps required.  Narrow --> more
       * taps More attenuation --> more taps.  Window type determines maximum
       * attenuation and passband ripple.
       *
       * \param gain                overall gain of filter (typically 1.0)
       * \param sampling_freq       sampling freq (Hz)
       * \param low_cutoff_freq     center of transition band (Hz)
       * \param high_cutoff_freq    center of transition band (Hz)
       * \param transition_width    width of transition band (Hz).
       * \param attenuation_dB      out of band attenuation
       * \param window              one of firdes::win_type
       * \param beta                parameter for Kaiser window
       */
      static std::vector<float>
      band_reject_2(double gain,
        double sampling_freq,
        double low_cutoff_freq,	// Hz beginning transition band
        double high_cutoff_freq,	// Hz beginning transition band
        double transition_width,	// Hz width of transition band
        double attenuation_dB,    // out of band attenuation dB
        win_type window = WIN_HAMMING,
        double beta = 6.76);	// used only with Kaiser

      /*!\brief design a Hilbert Transform Filter
       *
       * \param ntaps           number of taps, must be odd
       * \param windowtype      one kind of firdes::win_type
       * \param beta            parameter for Kaiser window
       */
      static std::vector<float>
      hilbert(unsigned int ntaps = 19,
        win_type windowtype = WIN_RECTANGULAR,
        double beta = 6.76);

      /*!
       * \brief design a Root Cosine FIR Filter (do we need a window?)
       *
       * \param gain            overall gain of filter (typically 1.0)
       * \param sampling_freq   sampling freq (Hz)
       * \param symbol_rate     symbol rate, must be a factor of sample rate
       * \param alpha           excess bandwidth factor
       * \param ntaps           number of taps
       */
      static std::vector<float>
      root_raised_cosine(double gain,
        double sampling_freq,
        double symbol_rate, // Symbol rate, NOT bitrate (unless BPSK)
        double alpha,       // Excess Bandwidth Factor
        int ntaps);

      /*!
       * \brief design a Gaussian filter
       *
       * \param gain    overall gain of filter (typically 1.0)
       * \param spb     symbol rate, must be a factor of sample rate
       * \param bt      bandwidth to bitrate ratio
       * \param ntaps   number of taps
       */
      static std::vector<float>
      gaussian(double gain,
        double spb,
        double bt,     // Bandwidth to bitrate ratio
        int ntaps);

    private:
      static double bessi0(double x);
      static void sanity_check_1f(double sampling_freq, double f1,
				  double transition_width);
      static void sanity_check_2f(double sampling_freq, double f1, double f2,
				  double transition_width);
      static void sanity_check_2f_c(double sampling_freq, double f1, double f2,
				    double transition_width);

      static int compute_ntaps(double sampling_freq,
			       double transition_width,
			       win_type window_type, double beta);

      static int compute_ntaps_windes(double sampling_freq,
				      double transition_width,
				      double attenuation_dB);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* _FILTER_FIRDES_H_ */
