//
//  Copyright 2004,2005,2009 Free Software Foundation, Inc.
//
//  This file is part of GNU Radio
//
//  SPDX-License-Identifier: GPL-3.0-or-later
//
//

#include <tuple>
#include <vector>

// Routines for designing optimal FIR filters.

// For a great intro to how all this stuff works, see section 6.6 of
// "Digital Signal Processing: A Practical Approach", Emmanuael C. Ifeachor
// and Barrie W. Jervis, Adison-Wesley, 1993.  ISBN 0-201-54413-X.

namespace gr {
namespace kernel {
namespace filter {

class optfir
{
public:
    /**
     * @brief FIR lowpass filter length estimator.
     *
         FIR lowpass filter length estimator.  freq1 and freq2 are
        normalized to the sampling frequency.  delta_p is the passband
        deviation (ripple), delta_s is the stopband deviation (ripple).

        Note, this works for high pass filters too (freq1 > freq2), but
        doesn't work well if the transition is near f == 0 or f == fs/2

        From Herrmann et al (1973), Practical design rules for optimum
        finite impulse response filters.  Bell System Technical J., 52, 769-99
     *
     * @param freq1
     * @param freq2
     * @param delta_p
     * @param delta_s
     * @return size_t
     */
    static double lporder(double freq1, double freq2, double delta_p, double delta_s);

    /**
     * @brief FIR order estimator (lowpass, highpass, bandpass, mulitiband).
     *
        (n, fo, ao, w) = remezord (f, a, dev)
        (n, fo, ao, w) = remezord (f, a, dev, fs)

        (n, fo, ao, w) = remezord (f, a, dev) finds the approximate order,
        normalized frequency band edges, frequency band amplitudes, and
        weights that meet input specifications f, a, and dev, to use with
        the remez command.

        * f is a sequence of frequency band edges (between 0 and Fs/2, where
          Fs is the sampling frequency), and a is a sequence specifying the
          desired amplitude on the bands defined by f. The length of f is
          twice the length of a, minus 2. The desired function is
          piecewise constant.

        * dev is a sequence the same size as a that specifies the maximum
          allowable deviation or ripples between the frequency response
          and the desired amplitude of the output filter, for each band.

        Use remez with the resulting order n, frequency sequence fo,
        amplitude response sequence ao, and weights w to design the filter b
        which approximately meets the specifications given by remezord
        input parameters f, a, and dev:

        b = remez (n, fo, ao, w)

        (n, fo, ao, w) = remezord (f, a, dev, Fs) specifies a sampling frequency Fs.

        Fs defaults to 2 Hz, implying a Nyquist frequency of 1 Hz. You can
        therefore specify band edges scaled to a particular applications
        sampling frequency.

        In some cases remezord underestimates the order n. If the filter
        does not meet the specifications, try a higher order such as n+1
        or n+2.
     *
     * @return std::tuple<size_t, double, double, double>
     */
    static std::
        tuple<size_t, std::vector<double>, std::vector<double>, std::vector<double>>
        remezord(std::vector<double> fcuts,
                 std::vector<double> mags,
                 std::vector<double> devs,
                 double fsamp = 2);

    /**
     * @brief Convert a stopband attenuation in dB to an absolute value
     *
     * @return double
     */
    static double stopband_atten_to_dev(double atten_db);

    /**
     * @brief Convert passband ripple spec expressed in dB to an absolute value
     *
     * @return double
     */
    static double passband_ripple_to_dev(double ripple_db);

    /**
     * @brief     Builds a low pass filter.
        Args:
            gain: Filter gain in the passband (linear)
            Fs: Sampling rate (sps)
            freq1: End of pass band (in Hz)
            freq2: Start of stop band (in Hz)
            passband_ripple_db: Pass band ripple in dB (should be small, < 1)
            stopband_atten_db: Stop band attenuation in dB (should be large, >= 60)
            nextra_taps: Extra taps to use in the filter (default=2)
     */

    static std::vector<double> low_pass(double gain,
                                        double Fs,
                                        double freq1,
                                        double freq2,
                                        double passband_ripple_db,
                                        double stopband_atten_db,
                                        size_t nextra_taps = 2);

    // FIXME - rest of optfir.py
};

} // namespace filter
} // namespace kernel
} // namespace gr