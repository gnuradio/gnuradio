/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_FRACTIONAL_DELAY_H
#define INCLUDED_CHANNELS_FRACTIONAL_DELAY_H

#include <gnuradio/logger.h>
#include <gnuradio/math.h>
#include <cmath>
#include <vector>

namespace gr {
namespace channels {

// Tap gains mag * sinc(k - delay) of a path at a fractional delay. With
// delay = n + f, sin(pi (k - delay)) = -(-1)^(k - n) sin(pi f), so sin() is
// evaluated once per path rather than once per tap.
class sinc_taps
{
public:
    sinc_taps(float delay, float mag)
        : d_int_delay(static_cast<long>(std::round(delay))),
          d_frac_delay(delay - std::round(delay)),
          d_mag(mag),
          d_mag_sin_frac(mag * std::sin(static_cast<float>(GR_M_PI) * d_frac_delay))
    {
    }

    // gain of tap k
    float operator()(size_t k) const
    {
        const long k_minus_n = static_cast<long>(k) - d_int_delay;
        const float dist = k_minus_n - d_frac_delay; // k - delay
        if (dist == 0) {
            return d_mag;
        }
        return ((k_minus_n & 1) ? d_mag_sin_frac : -d_mag_sin_frac) /
               (static_cast<float>(GR_M_PI) * dist);
    }

private:
    long d_int_delay;
    float d_frac_delay;
    float d_mag;
    float d_mag_sin_frac;
};

// paths whose delay can leave [0, ntaps - 1] are poorly interpolated
inline void check_delay_window(const gr::logger_ptr& logger,
                               const std::vector<float>& delays,
                               const std::vector<float>& maxdev,
                               unsigned int ntaps)
{
    for (size_t j = 0; j < delays.size(); j++) {
        const float dev = maxdev.empty() ? 0 : maxdev[j];
        if (delays[j] - dev < 0 || delays[j] + dev > ntaps - 1) {
            logger->warn("path {:d} delay {:g} (max deviation {:g}) extends outside "
                         "the {:d} tap window; its gain will be inaccurate",
                         j,
                         delays[j],
                         dev,
                         ntaps);
        }
    }
}

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_FRACTIONAL_DELAY_H */
