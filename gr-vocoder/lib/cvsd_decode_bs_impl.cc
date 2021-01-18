/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cvsd_decode_bs_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

cvsd_decode_bs::sptr cvsd_decode_bs::make(short min_step,
                                          short max_step,
                                          double step_decay,
                                          double accum_decay,
                                          int K,
                                          int J,
                                          short pos_accum_max,
                                          short neg_accum_max)
{
    return gnuradio::make_block_sptr<cvsd_decode_bs_impl>(
        min_step, max_step, step_decay, accum_decay, K, J, pos_accum_max, neg_accum_max);
}

cvsd_decode_bs_impl::cvsd_decode_bs_impl(short min_step,
                                         short max_step,
                                         double step_decay,
                                         double accum_decay,
                                         int K,
                                         int J,
                                         short pos_accum_max,
                                         short neg_accum_max)
    : sync_interpolator("vocoder_cvsd_decode_bs",
                        io_signature::make(1, 1, sizeof(unsigned char)),
                        io_signature::make(1, 1, sizeof(short)),
                        8),
      d_min_step(min_step),
      d_max_step(max_step),
      d_step_decay(step_decay),
      d_accum_decay(accum_decay),
      d_K(K),
      d_J(J),
      d_pos_accum_max(pos_accum_max),
      d_neg_accum_max(neg_accum_max),
      d_accum(0),
      d_loop_counter(1),
      d_runner(0),
      d_runner_mask(0),
      d_stepsize(min_step)
{
    if (d_K > 32)
        throw std::runtime_error("cvsd_decode_bs_impl: K must be <= 32");

    if (d_J > d_K)
        throw std::runtime_error("cvsd_decode_bs_impl: J must be <= K");
}

cvsd_decode_bs_impl::~cvsd_decode_bs_impl()
{
    // nothing else required in this example
}

unsigned char cvsd_decode_bs_impl::cvsd_bitwise_sum(unsigned int input)
{
    unsigned int temp = input;
    unsigned char bits = 0;

    while (temp) {
        temp = temp & (temp - 1);
        bits++;
    }
    return bits;
}

int cvsd_decode_bs_impl::cvsd_round(double input)
{
    double temp;
    temp = input + 0.5;
    temp = floor(temp);

    return (int)temp;
}

unsigned int cvsd_decode_bs_impl::cvsd_pow(short radix, short power)
{
    double d_radix = (double)radix;
    int i_power = (int)power;
    double output;

    output = pow(d_radix, i_power);
    return ((unsigned int)cvsd_round(output));
}

int cvsd_decode_bs_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    short* out = (short*)output_items[0];

    int i = 0;
    short output_short = 0;       // 2 bytes 0 .. 65,535
    unsigned char bit_count = 0;  // 1 byte, 0 .. 255
    unsigned int mask = 0;        // 4 bytes, 0 .. 4,294,967,295
    unsigned char input_byte = 0; //  1 bytes
    unsigned char input_bit = 0;  // 1 byte, 0 .. 255

    // Loop through each input data point
    for (i = 0; i < noutput_items / 8.0; i++) {
        input_byte = in[i];
        // Initialize bit counter
        bit_count = 0;

        while (bit_count < 8) {
            // Compute the Appropriate Mask
            mask = cvsd_pow(2, 7 - bit_count);

            // Pull off the corresponding bit
            input_bit = input_byte & mask;

            // Update the bit counter
            bit_count++;

            // Update runner with the next input bit
            // Runner is a shift-register; shift left, add on newest output bit
            d_runner = (d_runner << 1) | ((unsigned int)input_bit);

            // Run this only if you have >= J bits in your shift register
            if (d_loop_counter >= d_J) {
                // Update Step Size
                d_runner_mask = (cvsd_pow(2, d_J) - 1);
                if ((cvsd_bitwise_sum(d_runner & d_runner_mask) >= d_J) ||
                    (cvsd_bitwise_sum((~d_runner) & d_runner_mask) >= d_J)) {
                    // Runs of 1s and 0s
                    d_stepsize = std::min((short)(d_stepsize + d_min_step), d_max_step);
                } else {
                    // No runs of 1s and 0s
                    d_stepsize = std::max((short)cvsd_round(d_stepsize * d_step_decay),
                                          d_min_step);
                }
            }

            // Update Accum (i.e. the reference value)
            if (input_bit) {
                d_accum = d_accum + d_stepsize;
            } else {
                d_accum = d_accum - d_stepsize;
            }

            // Multiply by Accum_Decay
            d_accum = (cvsd_round(d_accum * d_accum_decay));

            // Check for overflow
            if (d_accum >= ((int)d_pos_accum_max)) {
                d_accum = (int)d_pos_accum_max;
            } else if (d_accum <= ((int)d_neg_accum_max)) {
                d_accum = (int)d_neg_accum_max;
            }

            // Find the output short to write to the file
            output_short = ((short)d_accum);

            if (d_loop_counter <= d_K) {
                d_loop_counter++;
            }

            *(out++) = output_short;
        } // while ()

    } // for()

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
