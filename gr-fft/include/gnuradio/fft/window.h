/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2008,2012,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FFT_WINDOW_H
#define INCLUDED_FFT_WINDOW_H

#include <gnuradio/fft/api.h>
#include <vector>
#include <cmath>
#include <gnuradio/gr_complex.h>

namespace gr {
  namespace fft {

    class FFT_API window {
    public:

      enum win_type {
        WIN_HAMMING = 0,         //!< Hamming window; max attenuation 53 dB
        WIN_HANN = 1,            //!< Hann window; max attenuation 44 dB
        WIN_BLACKMAN = 2,        //!< Blackman window; max attenuation 74 dB
        WIN_RECTANGULAR = 3,     //!< Basic rectangular window; max attenuation 21 dB
        WIN_KAISER = 4,          //!< Kaiser window; max attenuation see window::max_attenuation
        WIN_BLACKMAN_hARRIS = 5, //!< Blackman-harris window; max attenuation 92 dB
        WIN_BLACKMAN_HARRIS = 5, //!< alias to WIN_BLACKMAN_hARRIS for capitalization consistency
        WIN_BARTLETT = 6,        //!< Barlett (triangular) window; max attenuation 26 dB
        WIN_FLATTOP = 7,         //!< flat top window; useful in FFTs; max attenuation 93 dB
      };

      /*!
       * \brief Given a window::win_type, this tells you the maximum
       * attenuation you can expect.
       *
       * \details
       * For most windows, this is a set value. For the Kaiser window,
       * the attenuation is based on the value of beta. The actual
       * relationship is a piece-wise exponential relationship to
       * calculate beta from the desired attenuation and can be found
       * on page 542 of Oppenheim and Schafer (Discrete-Time Signal
       * Processing, 3rd edition). To simplify this function to solve
       * for A given beta, we use a linear form that is exact for
       * attenuation >= 50 dB.
       *
       * For an attenuation of 50 dB, beta = 4.55.
       *
       * For an attenuation of 70 dB, beta = 6.76.
       *
       * \param type The window::win_type enumeration of the window type.
       * \param beta Beta value only used for the Kaiser window.
       */
      static double max_attenuation(win_type type, double beta=6.76);

     /*!
       * \brief Helper function to build cosine-based windows. 3-coefficient version.
       */
      static std::vector<float> coswindow(int ntaps, float c0, float c1, float c2);

      /*!
       * \brief Helper function to build cosine-based windows. 4-coefficient version.
       */
      static std::vector<float> coswindow(int ntaps, float c0, float c1, float c2, float c3);

      /*!
       * \brief Helper function to build cosine-based windows. 5-coefficient version.
       */
      static std::vector<float> coswindow(int ntaps, float c0, float c1, float c2, float c3, float c4);

      /*!
       * \brief Build a rectangular window.
       *
       * Taps are flat across the window.
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> rectangular(int ntaps);

      /*!
       * \brief Build a Hamming window.
       *
       * See:
       * <pre>
       *   A. V. Oppenheim and R. W. Schafer, "Discrete-Time
       *   Signal Processing," Upper Saddle River, N.J.: Prentice
       *   Hall, 2010, pp. 535-538.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> hamming(int ntaps);

      /*!
       * \brief Build a Hann window (sometimes known as Hanning).
       *
       * See:
       * <pre>
       *   A. V. Oppenheim and R. W. Schafer, "Discrete-Time
       *   Signal Processing," Upper Saddle River, N.J.: Prentice
       *   Hall, 2010, pp. 535-538.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> hann(int ntaps);

      /*!
       * \brief Alias to build a Hann window.
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> hanning(int ntaps);

      /*!
       * \brief Build an exact Blackman window.
       *
       * See:
       * <pre>
       *   A. V. Oppenheim and R. W. Schafer, "Discrete-Time
       *   Signal Processing," Upper Saddle River, N.J.: Prentice
       *   Hall, 2010, pp. 535-538.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> blackman(int ntaps);

      /*!
       * \brief Build Blackman window, variation 1.
       */
      static std::vector<float> blackman2(int ntaps);

      /*!
       * \brief Build Blackman window, variation 2.
       */
      static std::vector<float> blackman3(int ntaps);

      /*!
       * \brief Build Blackman window, variation 3.
       */
      static std::vector<float> blackman4(int ntaps);

      /*!
       * \brief Build a Blackman-harris window with a given attenuation.
       *
       * <pre>
       *     f. j. harris, "On the use of windows for harmonic analysis
       *     with the discrete Fourier transforms," Proc. IEEE, Vol. 66,
       *     ppg. 51-83, Jan. 1978.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.

       * \param atten Attenuation factor. Must be [61, 67, 74, 92].
       *              See the above paper for details.
       */
      static std::vector<float> blackman_harris(int ntaps, int atten=92);

      /*!
       * Alias to gr::fft::window::blakcman_harris.
       */
      static std::vector<float> blackmanharris(int ntaps, int atten=92);

      /*!
       * \brief Build a Nuttall (or Blackman-Nuttall) window.
       *
       * See: http://en.wikipedia.org/wiki/Window_function#Blackman.E2.80.93Nuttall_window
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> nuttall(int ntaps);

      /*!
       * Deprecated: use nuttall window instead.
       */
      static std::vector<float> nuttal(int ntaps);

      /*!
       * \brief Alias to the Nuttall window.
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> blackman_nuttall(int ntaps);

      /*!
       * Deprecated: use blackman_nuttall window instead.
       */
      static std::vector<float> blackman_nuttal(int ntaps);

      /*!
       * \brief Build a Nuttall continuous first derivative window.
       *
       * See: http://en.wikipedia.org/wiki/Window_function#Nuttall_window.2C_continuous_first_derivative
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> nuttall_cfd(int ntaps);

      /*!
       * Deprecated: use nuttall_cfd window instead.
       */
      static std::vector<float> nuttal_cfd(int ntaps);

      /*!
       * \brief Build a flat top window.
       *
       * See: http://en.wikipedia.org/wiki/Window_function#Flat_top_window
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> flattop(int ntaps);

      /*!
       * \brief Build a Kaiser window with a given beta.
       *
       * See:
       * <pre>
       *     A. V. Oppenheim and R. W. Schafer, "Discrete-Time
       *     Signal Processing," Upper Saddle River, N.J.: Prentice
       *     Hall, 2010, pp. 541-545.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       * \param beta Shaping parameter of the window. See the
       *        discussion in Oppenheim and Schafer.
       */
      static std::vector<float> kaiser(int ntaps, double beta);

      /*!
       * \brief Build a Barlett (triangular) window.
       *
       * See:
       * <pre>
       *   A. V. Oppenheim and R. W. Schafer, "Discrete-Time
       *   Signal Processing," Upper Saddle River, N.J.: Prentice
       *   Hall, 2010, pp. 535-538.
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> bartlett(int ntaps);

      static std::vector<float> welch(int ntaps);

      /*!
       * \brief Build a Parzen (or de la Valle-Poussin) window.
       *
       * See:
       * <pre>
       *   A. D. Poularikas, "Handbook of Formulas and Tables for
       *   Signal Processing," Springer, Oct 28, 1998
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> parzen(int ntaps);

      /*!
       * \brief Build an exponential window with a given decay.
       *
       * See: http://en.wikipedia.org/wiki/Window_function#Exponential_or_Poisson_window
       *
       * \param ntaps Number of coefficients in the window.
       * \param d Decay of \p d dB over half the window length.
       */
      static std::vector<float> exponential(int ntaps, double d);

      /*!
       * \brief Build a Riemann window.
       *
       * See:
       * <pre>
       *   A. D. Poularikas, "Handbook of Formulas and Tables for
       *   Signal Processing," Springer, Oct 28, 1998
       * </pre>
       *
       * \param ntaps Number of coefficients in the window.
       */
      static std::vector<float> riemann(int ntaps);

      /*!
       * \brief Build a window using gr::fft::win_type to index the
       * type of window desired.
       *
       * \param type a gr::fft::win_type index for the type of window.
       * \param ntaps Number of coefficients in the window.
       * \param beta Used only for building Kaiser windows.
       */
      static std::vector<float> build(win_type type, int ntaps, double beta);
    };

  } /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_WINDOW_H */
