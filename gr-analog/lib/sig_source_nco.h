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
#include <cstring>
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
        // Interpret the modular phase as a signed angle without a sign branch.
        int64_t signed_phase;
        std::memcpy(&signed_phase, &phase, sizeof(signed_phase));
        return static_cast<double>(signed_phase) * (GR_M_PI / 0x1p63);
    }

    int32_t lookup_phase() const { return static_cast<int32_t>(d_phase >> 32); }

public:
    void set_phase(float angle) { d_phase = radians_to_phase(angle); }
    void set_freq(double angle_rate) { d_phase_inc = radians_to_phase(angle_rate); }
    float get_phase() const { return static_cast<float>(phase_to_radians(d_phase)); }
    double get_freq() const { return phase_to_radians(d_phase_inc); }
    // Phase divided by pi, retaining the full accumulator before conversion.
    double get_phase_normalized() const
    {
        int64_t phase;
        std::memcpy(&phase, &d_phase, sizeof(phase));
        return static_cast<double>(phase) * 0x1p-63;
    }
    bool phase_is_negative() const { return d_phase >> 63; }
    // Quarter turn for the complex waveforms: 0 is [0, pi/2), 1 is [pi/2, pi),
    // 2 is [-pi, -pi/2) and 3 is [-pi/2, 0). The +/-pi/2 boundaries are placed
    // where the previous comparison of the float get_phase() result switched;
    // rounding to float moves them about 1.6e-8 rad towards zero.
    unsigned quadrant() const
    {
        // Smallest phase word whose get_phase() is not below pi/2.
        constexpr int64_t quarter_turn = 0x3ffffff522cbdb00;
        int64_t phase;
        std::memcpy(&phase, &d_phase, sizeof(phase));
        if (phase >= 0)
            return phase < quarter_turn ? 0 : 1;
        return phase > -quarter_turn ? 3 : 2;
    }

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
        // Keep the accumulator local: complex output can inhibit alias analysis.
        uint64_t phase = d_phase;
        const uint64_t phase_inc = d_phase_inc;
        for (int i = 0; i < count; ++i) {
            const int32_t angle = static_cast<int32_t>(phase >> 32);
            output[i] = gr_complex(gr::fxpt::cos(angle) * amplitude,
                                   gr::fxpt::sin(angle) * amplitude);
            phase += phase_inc;
        }
        d_phase = phase;
    }
};

} // namespace detail
} // namespace analog
} // namespace gr

#endif /* INCLUDED_ANALOG_SIG_SOURCE_NCO_H */
