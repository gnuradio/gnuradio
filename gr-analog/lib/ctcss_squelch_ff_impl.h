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

namespace gr {
namespace analog {

class ctcss_squelch_ff_impl : public ctcss_squelch_ff, squelch_base_ff_impl
{
private:
    float d_freq;
    float d_level;
    int d_len;
    int d_rate;
    bool d_mute;

    fft::goertzel* d_goertzel_l;
    fft::goertzel* d_goertzel_c;
    fft::goertzel* d_goertzel_r;

    static int find_tone(float freq);
    static void compute_freqs(const float& freq, float& f_l, float& f_r);

    void update_fft_params();

protected:
    virtual void update_state(const float& in);
    virtual bool mute() const { return d_mute; }

public:
    ctcss_squelch_ff_impl(
        int rate, float freq, float level, int len, int ramp, bool gate);
    ~ctcss_squelch_ff_impl();

    std::vector<float> squelch_range() const;
    float level() const { return d_level; }
    void set_level(float level);
    int len() const { return d_len; }
    float frequency() const { return d_freq; }
    void set_frequency(float frequency);

    int ramp() const { return squelch_base_ff_impl::ramp(); }
    void set_ramp(int ramp) { squelch_base_ff_impl::set_ramp(ramp); }
    bool gate() const { return squelch_base_ff_impl::gate(); }
    void set_gate(bool gate) { squelch_base_ff_impl::set_gate(gate); }
    bool unmuted() const { return squelch_base_ff_impl::unmuted(); }

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
    {
        return squelch_base_ff_impl::general_work(
            noutput_items, ninput_items, input_items, output_items);
    }
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_CTCSS_SQUELCH_FF_IMPL_H */
