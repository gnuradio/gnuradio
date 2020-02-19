/*
 * Copyright 1995 Phil Karn, KA9Q
 * Copyright 2008,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Generate metric tables for a soft-decision convolutional decoder
 * assuming gaussian noise on a PSK channel.
 *
 * Works from "first principles" by evaluating the normal probability
 * function and then computing the log-likelihood function
 * for every possible received symbol value
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Symbols are offset-binary, with 128 corresponding to an erased (no
 * information) symbol
 */
#define OFFSET 128

#include <gnuradio/math.h>

#include <cmath>
#include <cstdlib>

/* Normal function integrated from -Inf to x. Range: 0-1 */
#define normal(x) (0.5 + 0.5 * erf((x) / GR_M_SQRT2))

namespace gr {
namespace fec {

/* Generate log-likelihood metrics for 8-bit soft quantized channel
 * assuming AWGN and BPSK
 */
void gen_met(int mettab[2][256], /* Metric table, [sent sym][rx symbol] */
             int amp,            /* Signal amplitude, units */
             double esn0,        /* Es/N0 ratio in dB */
             double bias,        /* Metric bias; 0 for viterbi, rate for sequential */
             int scale)          /* Scale factor */
{
    double noise;
    int s, bit;
    double metrics[2][256];
    double p0, p1;

    /* Es/N0 as power ratio */
    esn0 = pow(10., esn0 / 10);

    noise = 0.5 / esn0;  /* only half the noise for BPSK */
    noise = sqrt(noise); /* noise/signal Voltage ratio */

    /* Zero is a special value, since this sample includes all
     * lower samples that were clipped to this value, i.e., it
     * takes the whole lower tail of the curve
     */
    p1 = normal(((0 - OFFSET + 0.5) / amp - 1) / noise); /* P(s|1) */

    /* Prob of this value occurring for a 0-bit */ /* P(s|0) */
    p0 = normal(((0 - OFFSET + 0.5) / amp + 1) / noise);
    metrics[0][0] = ::log2(2 * p0 / (p1 + p0)) - bias;
    metrics[1][0] = ::log2(2 * p1 / (p1 + p0)) - bias;

    for (s = 1; s < 255; s++) {
        /* P(s|1), prob of receiving s given 1 transmitted */
        p1 = normal(((s - OFFSET + 0.5) / amp - 1) / noise) -
             normal(((s - OFFSET - 0.5) / amp - 1) / noise);

        /* P(s|0), prob of receiving s given 0 transmitted */
        p0 = normal(((s - OFFSET + 0.5) / amp + 1) / noise) -
             normal(((s - OFFSET - 0.5) / amp + 1) / noise);

#ifdef notdef
        printf("P(%d|1) = %lg, P(%d|0) = %lg\n", s, p1, s, p0);
#endif
        metrics[0][s] = ::log2(2 * p0 / (p1 + p0)) - bias;
        metrics[1][s] = ::log2(2 * p1 / (p1 + p0)) - bias;
    }
    /* 255 is also a special value */
    /* P(s|1) */
    p1 = 1 - normal(((255 - OFFSET - 0.5) / amp - 1) / noise);
    /* P(s|0) */
    p0 = 1 - normal(((255 - OFFSET - 0.5) / amp + 1) / noise);

    metrics[0][255] = ::log2(2 * p0 / (p1 + p0)) - bias;
    metrics[1][255] = ::log2(2 * p1 / (p1 + p0)) - bias;
#ifdef notdef
    /* The probability of a raw symbol error is the probability
     * that a 1-bit would be received as a sample with value
     * 0-128. This is the offset normal curve integrated from -Inf to 0.
     */
    printf("symbol Pe = %lg\n", normal(-1 / noise));
#endif
    for (bit = 0; bit < 2; bit++) {
        for (s = 0; s < 256; s++) {
            /* Scale and round to nearest integer */
            mettab[bit][s] = floor(metrics[bit][s] * scale + 0.5);
#ifdef notdef
            printf("metrics[%d][%d] = %lg, mettab = %d\n",
                   bit,
                   s,
                   metrics[bit][s],
                   mettab[bit][s]);
#endif
        }
    }
}
} // namespace fec
} // namespace gr
