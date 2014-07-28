/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GNURADIO_FEC_BER_TOOLS_H
#define INCLUDED_GNURADIO_FEC_BER_TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BERMINFRAMES (10000)
#define BERMINERRORS (100)
#define BERMAXBITS   (1000000000)

/*!
 * Add BPSK gaussian noise with standard deviation equal to sigma to a
 * floating point input buffer.
 *
 * \param inbuffer (float*) buffer containing data to receive additive
 *                 gaussian noise
 * \param buffsize (int) size of \p inbuffer
 * \param sigma    (float) noise power of the guassian random variables
 */
void gaussnoise(float *inbuffer, int buffsize, float sigma);

/*!
 * Compute the number of bit differences between input buffers
 *
 * \param inbuffer1 input stream 1 to compare against \p inbuffer2
 * \param inbuffer2 input stream 2 to be compared against
 * \param buffsize  number of elements in each buffer
 */
int compber(unsigned char *inbuffer1, unsigned char *inbuffer2, int buffsize);

/*!
 * Generate a random buffer of data
 *
 * \param databuffer pointer to buffer containing random data
 * \param buffsize   number of elements in each buffer
 */
void randbuffer(unsigned char *databuffer, int buffsize, int charout);

/*!
 * Pack the character buffer
 *
 * \param databuffer pointer to buffer containing unpacked chars
 * \param buffsize   number of elements in each buffer
 */
void char2bin(unsigned char *inbuffer, int buffsize);

#endif  /* INCLUDED_GNURADIO_FEC_BER_TOOLS_H */


