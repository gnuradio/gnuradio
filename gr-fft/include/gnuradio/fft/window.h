/* -*- c++ -*- */
/*
 * Copyright 2002,2007,2008,2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FFT_WINDOW_H
#define INCLUDED_FFT_WINDOW_H

#include <gnuradio/fft/api.h>
#include <gnuradio/gr_complex.h>
#include <cmath>
#include <vector>

namespace gr {
namespace fft {

class FFT_API window
{
public:
    // illegal value for any window that requires a parameter
    static constexpr double INVALID_WIN_PARAM = -1;

    enum win_type {
        WIN_NONE = -1,       //!< don't use a window
        WIN_HAMMING = 0,     //!< Hamming window; max attenuation 53 dB
        WIN_HANN = 1,        //!< Hann window; max attenuation 44 dB
        WIN_HANNING = 1,     //!< alias to WIN_HANN
        WIN_BLACKMAN = 2,    //!< Blackman window; max attenuation 74 dB
        WIN_RECTANGULAR = 3, //!< Basic rectangular window; max attenuation 21 dB
        WIN_KAISER = 4, //!< Kaiser window; max attenuation see window::max_attenuation
        WIN_BLACKMAN_hARRIS = 5, //!< Blackman-harris window; max attenuation 92 dB
        WIN_BLACKMAN_HARRIS =
            5,            //!< alias to WIN_BLACKMAN_hARRIS for capitalization consistency
        WIN_BARTLETT = 6, //!< Barlett (triangular) window; max attenuation 26 dB
        WIN_FLATTOP = 7,  //!< flat top window; useful in FFTs; max attenuation 93 dB
        WIN_NUTTALL = 8,  //!< Nuttall window; max attenuation 114 dB
        WIN_BLACKMAN_NUTTALL = 8, //!< Nuttall window; max attenuation 114 dB
        WIN_NUTTALL_CFD =
            9, //!< Nuttall continuous-first-derivative window; max attenuation 112 dB
        WIN_WELCH = 10,  //!< Welch window; max attenuation 31 dB
        WIN_PARZEN = 11, //!< Parzen window; max attenuation 56 dB
        WIN_EXPONENTIAL =
            12, //!< Exponential window; max attenuation see window::max_attenuation
        WIN_RIEMANN = 13, //!< Riemann window; max attenuation 39 dB
        WIN_GAUSSIAN =
            14,         //!< Gaussian window; max attenuation see window::max_attenuation
        WIN_TUKEY = 15, //!< Tukey window; max attenuation see window::max_attenuation
    };

    /*!
     * \brief Given a window::win_type, this tells you the maximum
     * attenuation (really the maximum approximation error) you can expect.
     *
     * \details
     * For most windows, this is a set value. For the Kaiser, Exponential, Gaussian, and
     * Tukey windows, the attenuation is based on the value of a provided parameter.
     *
     * For the Kaiser window the actual relationship is a piece-wise exponential
     * relationship to calculate beta from the desired attenuation and can be found on
     * page 542 of Oppenheim and Schafer (Discrete-Time Signal Processing, 3rd edition).
     * To simplify this function to solve for A given beta, we use a linear form that is
     * exact for attenuation >= 50 dB. For an attenuation of 50 dB, beta = 4.55; for an
     * attenuation of 70 dB, beta = 6.76.
     *
     * Exponential attenuation is complicated to measure due to the irregular error ripple
     * structure, but it ranges between 23 and 26 dB depending on the decay factor; 26 dB
     * is a good bound.
     *
     * The Gaussian window should not be used for window based filter construction;
     * instead there is a dedicated gaussian filter construction function. There is no
     * meaningful way to measure approximation error 'delta' as shown in Fig 7.23 of
     * Oppenheim and Schafer (Discrete-Time Signal Processing, 3rd edition).
     *
     * Tukey windows provide attenuation that varies non-linearily between Rectangular (21
     * dB) and Hann (44 dB) windows.
     *
     * \param type The window::win_type enumeration of the window type.
     * \param param Parameter value used for Kaiser (beta), Exponential (d), Gaussian
     * (sigma) and Tukey (alpha) window creation.
     */
    static double max_attenuation(win_type type, double param = INVALID_WIN_PARAM);

    /*!
     * \brief Helper function to build cosine-based windows. 3-coefficient version.
     */
    static std::vector<float> coswindow(int ntaps, float c0, float c1, float c2);

    /*!
     * \brief Helper function to build cosine-based windows. 4-coefficient version.
     */
    static std::vector<float>
    coswindow(int ntaps, float c0, float c1, float c2, float c3);

    /*!
     * \brief Helper function to build cosine-based windows. 5-coefficient version.
     */
    static std::vector<float>
    coswindow(int ntaps, float c0, float c1, float c2, float c3, float c4);

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
    static std::vector<float> blackman_harris(int ntaps, int atten = 92);

    /*!
     * Alias to gr::fft::window::blackman_harris.
     */
    static std::vector<float> blackmanharris(int ntaps, int atten = 92);

    /*!
     * \brief Build a minimum 4-term Nuttall (or Blackman-Nuttall) window, referred to by
     * Heinzel G. et al. as a Nuttall4c window.
     *
     * See: A.H. Nuttall: 'Some windows with very good sidelobe behaviour', IEEE Trans. on
     * Acoustics, Speech and Signal Processing, Vol ASSP-29, figure 15
     *
     * See: 'Spectrum and spectral density estimation by the Discrete Fourier transform
     * (DFT), including a comprehensive list of window functions and some new flat-top
     * windows', February 15, 2002 https://holometer.fnal.gov/GH_FFT.pdf
     *
     * Also: http://en.wikipedia.org/wiki/Window_function#Blackman.E2.80.93Nuttall_window
     *
     * \param ntaps Number of coefficients in the window.
     */
    static std::vector<float> nuttall(int ntaps);

    /*!
     * \brief Alias to the Nuttall window.
     *
     * \param ntaps Number of coefficients in the window.
     */
    static std::vector<float> blackman_nuttall(int ntaps);

    /*!
     * \brief Build a Nuttall 4-term continuous first derivative window, referred to by
     * Heinzel G. et al. as a Nuttall4b window
     *
     * See: A.H. Nuttall: 'Some windows with very good sidelobe behaviour', IEEE Trans. on
     * Acoustics, Speech and Signal Processing, Vol ASSP-29, figure 12
     *
     * See: 'Spectrum and spectral density estimation by the Discrete Fourier transform
     * (DFT), including a comprehensive list of window functions and some new flat-top
     * windows', February 15, 2002 https://holometer.fnal.gov/GH_FFT.pdf
     *
     * Also:
     * http://en.wikipedia.org/wiki/Window_function#Nuttall_window.2C_continuous_first_derivative
     *
     * \param ntaps Number of coefficients in the window.
     */
    static std::vector<float> nuttall_cfd(int ntaps);

    /*!
     * \brief Build a flat top window per the SRS specification
     *
     * See:
     * <pre>
     *     Stanford Research Systems, "Model SR785 Dynamic Signal
     *     Analyzer: Operating Manual and Programming Reference,"
     *     2017, pp 2-13
     * </pre>
     *
     * Note: there are many flat top windows, and this implementation is different from
     * SciPY and Matlab which use the coefficients from D’Antona et al. "Digital Signal
     * Processing for Measurement Systems" with the following cosine coefficients: <pre>
     *     [0.21557895, 0.41663158, 0.277263158, 0.083578947, 0.006947368]
     * </pre>
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
     * \brief Build a Tukey window.
     * <pre>
     * Bloomfield, P. Fourier Analysis of Time Series: An Introduction. New York:
     * Wiley-Interscience, 2000, pp 69 (eqn 6.9)
     * </pre>
     *
     * \param ntaps Number of coefficients in the window.
     * \param alpha Shaping parameter for the Tukey window, an
     *        alpha of zero is equivalent to a rectangular
     *        window, an alpha of 1 is equivalent to Hann.
     */
    static std::vector<float> tukey(int ntaps, float alpha);

    /*!
     * \brief Build a Gaussian window using the equation
     * <pre>
     * exp(-(1/2) * (n/sigma)^2)
     * </pre>
     *
     * \param ntaps Number of coefficients in the window.
     * \param sigma Standard deviation of gaussian distribution.
     */
    static std::vector<float> gaussian(int ntaps, float sigma);

    /*!
     * \brief Build a window using gr::fft::win_type to index the
     * type of window desired.
     *
     * \param type a gr::fft::win_type index for the type of window.
     * \param ntaps Number of coefficients in the window.
     * \param param Parameter value used for Kaiser (beta), Exponential (d), Gaussian
     * (sigma) and Tukey (alpha) window creation. \param normalize If true, return a
     * window with unit power
     */
    static std::vector<float> build(win_type type,
                                    int ntaps,
                                    double param = INVALID_WIN_PARAM,
                                    const bool normalize = false);
};

} /* namespace fft */
} /* namespace gr */

#endif /* INCLUDED_FFT_WINDOW_H */
