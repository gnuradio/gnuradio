/* -*- c++ -*- */
/*
 * Copyright 2023 Martin Hübner
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "afsk_mod_bf_impl.h"
#include "gnuradio/digital/afsk_mod_bf.h"
#include <gnuradio/endianness.h>
#include <gnuradio/io_signature.h>
#include <cstdint>

namespace gr {
namespace digital {

using input_type = uint8_t;
using output_type = float;

afsk_mod_bf::sptr afsk_mod_bf::make(int samp_rate,
                                    float freq_zero,
                                    float freq_one,
                                    int bit_rate,
                                    bool bits,
                                    endianness_t endianness)
{
    return gnuradio::make_block_sptr<afsk_mod_bf_impl>(
        samp_rate, freq_zero, freq_one, bit_rate, bits, endianness);
}


int afsk_mod_bf_impl::no_of_output(bool bits, int samp_rate, int bit_rate)
{
    if (bits) {
        // input is sliced into bits already
        return (samp_rate / bit_rate);
    } else {
        // input is bytes
        return 8 * (samp_rate / bit_rate);
    }
}


/*
 * The private constructor
 */
afsk_mod_bf_impl::afsk_mod_bf_impl(int samp_rate,
                                   float freq_zero,
                                   float freq_one,
                                   int bit_rate,
                                   bool bits,
                                   endianness_t endianness)
    : gr::sync_interpolator(
          "afsk_mod_bf",
          gr::io_signature::make(
              1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
          gr::io_signature::make(
              1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)),
          no_of_output(bits, samp_rate, bit_rate))
{
    this->samp_rate = samp_rate;
    this->freq_zero = freq_zero;
    this->freq_one = freq_one;
    this->bit_rate = bit_rate;
    this->bits = bits;
    this->endianness = endianness;

    this->m = 0.0;
    this->t = 0;
}

/*
 * Our virtual destructor.
 */
afsk_mod_bf_impl::~afsk_mod_bf_impl() {}

float afsk_mod_bf_impl::calc_center_freq(float freq_zero, float freq_one)
{
    return (freq_zero + freq_one) / 2;
}

float afsk_mod_bf_impl::calc_delta_freq(float freq_zero, float freq_one)
{
    float freq_center = calc_center_freq(freq_zero, freq_one);
    int freq_delta = 0;

    if (freq_zero < freq_one) {
        freq_delta = freq_one - freq_center;
    } else {
        freq_delta = freq_zero - freq_center;
    }

    return freq_delta;
}

void afsk_mod_bf_impl::nrz(std::vector<int>& bits)
{
    for (size_t bit = 0; bit < bits.size(); bit++) {
        bits[bit] = bits[bit] * 2 - 1;
    }
}


int afsk_mod_bf_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);


    float freq_center = calc_center_freq(freq_zero, freq_one);
    float freq_delta = calc_delta_freq(freq_zero, freq_one);

    std::vector<int> sliced_bits;
    // preallocate vector for better performance
    sliced_bits.reserve(noutput_items);

    if (!bits) {
        // Input is not bits, but bytes: Iterate over input bytes and slice
        // them into separate bits. Store them in a vector afterwards.

        switch (endianness) {
        case GR_MSB_FIRST:
            for (int byte = 0; byte < noutput_items / 8; byte++) {
                uint8_t raw_byte = in[byte];

                for (int i = 7; i >= 0; --i) {
                    int bit = (raw_byte >> i) & 1;
                    sliced_bits.push_back(bit);
                }
            }
            break;
        case GR_LSB_FIRST:
            for (int byte = 0; byte < noutput_items / 8; byte++) {
                uint8_t raw_byte = in[byte];

                for (int i = 0; i <= 7; ++i) {
                    int bit = (raw_byte >> i) & 1;
                    sliced_bits.push_back(bit);
                }
            }
            break;
        default:
            this->d_logger->error("unknown endianness");
            throw std::runtime_error("unknown endianness");
        }
    } else {
        // move input_bits into sliced bits
        for (int bit = 0; bit < noutput_items; bit++) {
            sliced_bits.push_back(in[bit]);
        }
    }

    /* use implementation from https://notblackmagic.com/bitsnpieces/afsk/
     * as blueprint
     */

    // Convert bits to NRZ signal
    nrz(sliced_bits);

    // each bit will get a certain amount of samples in the output signal
    int steps = samp_rate / bit_rate;

    // These get used inside the loop. Reside here for avoiding multiple
    // divisions (increases performance)
    double fd_sr = freq_delta / samp_rate;
    double fc_sr = freq_center / samp_rate;

    for (int i = 1; i < noutput_items; i++) {
        // calculate the source bits indices in bit-vector on basis of
        // output-buffer-size. These are needed for the integral
        int curr_i = i / steps;
        int prev_i = (i - 1) / steps;

        // add up integrals using the trapezoidal rule
        this->m += (sliced_bits[prev_i] + sliced_bits[curr_i] / 2.0);

        // write data point of audio curve to output-buffer
        out[i] = cos(2 * M_PI * this->t * fc_sr - 2 * M_PI * m * fd_sr);
        this->t++;
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
