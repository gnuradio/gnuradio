/* -*- c++ -*- */
/*
 * Copyright 2023 Martin Hübner
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DIGITAL_AFSK_MOD_BF_IMPL_H
#define INCLUDED_DIGITAL_AFSK_MOD_BF_IMPL_H

#include <gnuradio/digital/afsk_mod_bf.h>

namespace gr {
namespace digital {

class afsk_mod_bf_impl : public afsk_mod_bf
{
private:
    int samp_rate;
    float freq_zero;
    float freq_one;
    int bit_rate;
    bool bits;
    endianness_t endianness;

    // holds the integral of the signal so far.
    double m;
    // counts samples of generated signals. Overflow shouldn't be a problem, as
    // even with 96kHz sample rate, this would be easily enough for 6109889 Years.
    uint64_t t;

public:
    afsk_mod_bf_impl(int samp_rate,
                     float freq_zero,
                     float freq_one,
                     int bit_rate,
                     bool bits,
                     endianness_t endianness);
    ~afsk_mod_bf_impl();

    /**
     * The number of output-bytes depends on the input type: If bytes are the input,
     * block will slice itself, having higher output. I bits are the input, it won't.
     */
    static int no_of_output(bool bits, int samp_rate, int bit_rate);

    /**
     * Calculates the center frequency of two given input frequencies.
     */
    float calc_center_freq(float freq_zero, float freq_one);

    /**
     * Transform the bits in the input signal into a Non-Return-Zero signal.
     *
     * That means turning all 0's to a -1.
     */
    void nrz(std::vector<int>& bits);

    /**
     * Get the difference of the center frequency and mark & space frequencies.
     */
    float calc_delta_freq(float freq_zero, float freq_center);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_AFSK_MOD_BF_IMPL_H */
