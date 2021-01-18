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

#include "cvsd_encode_sb_impl.h"
#include <gnuradio/io_signature.h>
#include <climits>

namespace gr {
namespace vocoder {

cvsd_encode_sb::sptr cvsd_encode_sb::make(short min_step,
                                          short max_step,
                                          double step_decay,
                                          double accum_decay,
                                          int K,
                                          int J,
                                          short pos_accum_max,
                                          short neg_accum_max)
{
    return gnuradio::make_block_sptr<cvsd_encode_sb_impl>(
        min_step, max_step, step_decay, accum_decay, K, J, pos_accum_max, neg_accum_max);
}

cvsd_encode_sb_impl::cvsd_encode_sb_impl(short min_step,
                                         short max_step,
                                         double step_decay,
                                         double accum_decay,
                                         int K,
                                         int J,
                                         short pos_accum_max,
                                         short neg_accum_max)
    : sync_decimator("vocoder_cvsd_encode_sb",
                     io_signature::make(1, 1, sizeof(short)),
                     io_signature::make(1, 1, sizeof(unsigned char)),
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
      d_stepsize(min_step)
{
    if (d_K > 32)
        throw std::runtime_error("cvsd_decode_bs_impl: K must be <= 32");

    if (d_J > d_K)
        throw std::runtime_error("cvsd_decode_bs_impl: J must be <= K");
}

cvsd_encode_sb_impl::~cvsd_encode_sb_impl()
{
    // nothing else required in this example
}

unsigned char cvsd_encode_sb_impl::cvsd_bitwise_sum(unsigned int input)
{
    unsigned int temp = input;
    unsigned char bits = 0;

    while (temp) {
        temp = temp & (temp - 1);
        bits++;
    }
    return bits;
}

int cvsd_encode_sb_impl::cvsd_round(double input)
{
    double temp;
    temp = input + 0.5;
    temp = floor(temp);

    return (int)temp;
}

unsigned int cvsd_encode_sb_impl::cvsd_pow(short radix, short power)
{
    double d_radix = (double)radix;
    int i_power = (int)power;
    double output;

    output = pow(d_radix, i_power);
    return ((unsigned int)cvsd_round(output));
}

int cvsd_encode_sb_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const short* in = (const short*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    unsigned short i = 0;          // 2 bytes, 0 .. 65,535
    unsigned char output_bit = 0;  // 1 byte, 0 .. 255
    unsigned char output_byte = 0; // 1 bytes 0.255
    unsigned char bit_count = 0;   // 1 byte, 0 .. 255
    unsigned int mask = 0;         // 4 bytes, 0 .. 4,294,967,295

    // Loop through each input data point
    for (i = 0; i < noutput_items * 8; i++) {
        if ((int)in[i] >= d_accum) { // Note:  sign((data(n)-accum))
            output_bit = 1;
        } else {
            output_bit = 0;
        }

        // Update Accum (i.e. the reference value)
        if (output_bit) {
            d_accum = d_accum + d_stepsize;
            // printf("Adding %d to the accum; the result is: %d.\n", d_stepsize,
            // d_accum);
        } else {
            d_accum = d_accum - d_stepsize;
            // printf("Subtracting %d to the accum; the result is: %d.\n", d_stepsize,
            // d_accum);
        }

        // Multiply by Accum_Decay
        d_accum = (cvsd_round(d_accum * d_accum_decay));

        // Check for overflow
        if (d_accum >= ((int)d_pos_accum_max)) {
            d_accum = (int)d_pos_accum_max;
        } else if (d_accum <= ((int)d_neg_accum_max)) {
            d_accum = (int)d_neg_accum_max;
        }

        // Update runner with the last output bit
        // Update Step Size
        if (d_loop_counter >=
            d_J) { // Run this only if you have >= J bits in your shift register
            mask = (cvsd_pow(2, d_J) - 1);
            if ((cvsd_bitwise_sum(d_runner & mask) >= d_J) ||
                (cvsd_bitwise_sum((~d_runner) & mask) >= d_J)) {
                // Runs of 1s and 0s
                d_stepsize = std::min((short)(d_stepsize + d_min_step), d_max_step);
            } else {
                // No runs of 1s and 0s
                d_stepsize =
                    std::max((short)cvsd_round(d_stepsize * d_step_decay), d_min_step);
            }
        }

        // Runner is a shift-register; shift left, add on newest output bit
        d_runner = (d_runner << 1) | ((unsigned int)output_bit);

        // Update the output type; shift left, add on newest output bit
        // If you have put in 8 bits, output it as a byte
        output_byte = (output_byte << 1) | output_bit;
        bit_count++;

        if (d_loop_counter <= d_K) {
            d_loop_counter++;
        }

        // If you have put 8 bits, output and clear.
        if (bit_count == 8) {
            // Read in short from the file
            *(out++) = output_byte;

            // Reset the bit_count
            bit_count = 0;
            output_byte = 0;
        }
    } // While

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
