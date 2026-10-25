/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ANALOG_SIG_SOURCE_NCO_H
#define INCLUDED_ANALOG_SIG_SOURCE_NCO_H

#include <gnuradio/fxpt.h>
#include <gnuradio/gr_complex.h>
#include <gnuradio/math.h>
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace gr {
namespace analog {
namespace detail {

// Private oscillator for Signal Source. The low phase bits preserve tuning
// precision; the existing sine table consumes only the upper 32 bits.
class sig_source_nco
{
    uint64_t d_phase = 0;
    uint64_t d_phase_inc = 0;

    static uint64_t radians_to_phase(double angle)
    {
        if (!std::isfinite(angle))
            throw std::invalid_argument("NCO angle must be finite");
        const double scaled = std::remainder(angle, 2 * GR_M_PI) * (0x1p63 / GR_M_PI);
        if (scaled < 0)
            return static_cast<uint64_t>(static_cast<int64_t>(scaled));
        return static_cast<uint64_t>(scaled);
    }

    static double phase_to_radians(uint64_t phase)
    {
        if (phase & (uint64_t{ 1 } << 63))
            return -static_cast<double>(~phase + 1) * (GR_M_PI / 0x1p63);
        return static_cast<double>(phase) * (GR_M_PI / 0x1p63);
    }

    int32_t lookup_phase() const { return static_cast<int32_t>(d_phase >> 32); }

public:
    void set_phase(float angle) { d_phase = radians_to_phase(angle); }
    void set_freq(double angle_rate) { d_phase_inc = radians_to_phase(angle_rate); }
    float get_phase() const { return static_cast<float>(phase_to_radians(d_phase)); }
    double get_freq() const { return phase_to_radians(d_phase_inc); }

    void step() { d_phase += d_phase_inc; }
    void step(int n) { d_phase += d_phase_inc * static_cast<uint64_t>(n); }

    float sin() const { return gr::fxpt::sin(lookup_phase()); }
    float cos() const { return gr::fxpt::cos(lookup_phase()); }

    template <class T>
    void sin(T* output, int count, double amplitude = 1.0)
    {
        for (int i = 0; i < count; ++i) {
            output[i] = static_cast<T>(sin() * amplitude);
            step();
        }
    }

    template <class T>
    void cos(T* output, int count, double amplitude = 1.0)
    {
        for (int i = 0; i < count; ++i) {
            output[i] = static_cast<T>(cos() * amplitude);
            step();
        }
    }

    void sincos(gr_complex* output, int count, double amplitude = 1.0)
    {
        for (int i = 0; i < count; ++i) {
            output[i] = gr_complex(cos() * amplitude, sin() * amplitude);
            step();
        }
    }
};

} // namespace detail
} // namespace analog
} // namespace gr

#endif /* INCLUDED_ANALOG_SIG_SOURCE_NCO_H */
