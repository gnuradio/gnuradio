/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_CTCSS_SQUELCH_FF_IMPL_H
#define INCLUDED_ANALOG_CTCSS_SQUELCH_FF_IMPL_H

#include "squelch_base_ff_impl.h"
#include <gnuradio/analog/ctcss_squelch_ff.h>
#include <gnuradio/fft/goertzel.h>
#include <memory>

namespace gr {
namespace analog {

class ctcss_squelch_ff_impl : public ctcss_squelch_ff, squelch_base_ff_impl
{
private:
    float d_freq;
    float d_level;
    int d_len;
    const int d_rate;
    bool d_mute;

    std::unique_ptr<fft::goertzel> d_goertzel_l;
    std::unique_ptr<fft::goertzel> d_goertzel_c;
    std::unique_ptr<fft::goertzel> d_goertzel_r;

    static int find_tone(float freq);
    static void compute_freqs(const float& freq, float& f_l, float& f_r);

    void update_fft_params();

protected:
    void update_state(const float& in) override;
    bool mute() const override { return d_mute; }

public:
    ctcss_squelch_ff_impl(
        int rate, float freq, float level, int len, int ramp, bool gate);
    ~ctcss_squelch_ff_impl() override;

    std::vector<float> squelch_range() const override;
    float level() const override { return d_level; }
    void set_level(float level) override;
    int len() const override { return d_len; }
    float frequency() const override { return d_freq; }
    void set_frequency(float frequency) override;

    int ramp() const override { return squelch_base_ff_impl::ramp(); }
    void set_ramp(int ramp) override { squelch_base_ff_impl::set_ramp(ramp); }
    bool gate() const override { return squelch_base_ff_impl::gate(); }
    void set_gate(bool gate) override { squelch_base_ff_impl::set_gate(gate); }
    bool unmuted() const override { return squelch_base_ff_impl::unmuted(); }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override
    {
        return squelch_base_ff_impl::general_work(
            noutput_items, ninput_items, input_items, output_items);
    }
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_CTCSS_SQUELCH_FF_IMPL_H */
