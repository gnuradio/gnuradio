/* --------------------------------------------------------------  */
/* (C)Copyright 2008 Free Software Foundation, Inc.                */
/* (C)Copyright 2001,2007,                                         */
/* International Business Machines Corporation,                    */
/* Sony Computer Entertainment, Incorporated,                      */
/* Toshiba Corporation,                                            */
/*                                                                 */
/* All Rights Reserved.                                            */
/*                                                                 */
/* Redistribution and use in source and binary forms, with or      */
/* without modification, are permitted provided that the           */
/* following conditions are met:                                   */
/*                                                                 */
/* - Redistributions of source code must retain the above copyright*/
/*   notice, this list of conditions and the following disclaimer. */
/*                                                                 */
/* - Redistributions in binary form must reproduce the above       */
/*   copyright notice, this list of conditions and the following   */
/*   disclaimer in the documentation and/or other materials        */
/*   provided with the distribution.                               */
/*                                                                 */
/* - Neither the name of IBM Corporation nor the names of its      */
/*   contributors may be used to endorse or promote products       */
/*   derived from this software without specific prior written     */
/*   permission.                                                   */
/*                                                                 */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND          */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,     */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF        */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR            */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT    */
/* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       */
/* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR    */
/* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              */
/* --------------------------------------------------------------  */
/* PROLOG END TAG zYx                                              */

#ifndef INCLUDED_LIBFFT_H
#define INCLUDED_LIBFFT_H

// must be defined before inclusion of fft_1d_r2.h
#define MAX_FFT_1D_SIZE 4096

/* fft_1d_r2
 * ---------
 * Performs a single precision, complex Fast Fourier Transform using 
 * the DFT (Discrete Fourier Transform) with radix-2 decimation in time. 
 * The input <in> is an array of complex numbers of length (1<<log2_size)
 * entries. The result is returned in the array of complex numbers specified
 * by <out>. Note: This routine can support an in-place transformation
 * by specifying <in> and <out> to be the same array.
 *
 * This implementation utilizes the Cooley-Tukey algorithm consisting 
 * of <log2_size> stages. The basic operation is the butterfly.
 *
 *          p --------------------------> P = p + q*Wi
 *                        \      /
 *                         \    /
 *                          \  /
 *                           \/
 *                           /\
 *                          /  \
 *                         /    \
 *               ____     /      \
 *          q --| Wi |-----------------> Q = p - q*Wi
 *               ----
 *
 * This routine also requires pre-computed twiddle values, W. W is an
 * array of single precision complex numbers of length 1<<(log2_size-2) 
 * and is computed as follows:
 *
 *	for (i=0; i<n/4; i++)
 *	    W[i].real =  cos(i * 2*PI/n);
 *	    W[i].imag = -sin(i * 2*PI/n);
 *      }
 *
 * This array actually only contains the first half of the twiddle
 * factors. Due for symmetry, the second half of the twiddle factors
 * are implied and equal:
 *
 *	for (i=0; i<n/4; i++)
 *	    W[i+n/4].real =  W[i].imag =  sin(i * 2*PI/n);
 *	    W[i+n/4].imag = -W[i].real = -cos(i * 2*PI/n);
 *      }
 *
 * Further symmetry allows one to generate the twiddle factor table 
 * using half the number of trig computations as follows:
 *
 *      W[0].real = 1.0;
 *      W[0].imag = 0.0;
 *	for (i=1; i<n/4; i++)
 *	    W[i].real =  cos(i * 2*PI/n);
 *	    W[n/4 - i].imag = -W[i].real;
 *      }
 *
 * The complex numbers are packed into quadwords as follows:
 *
 *    quadword			      complex
 *  array element                   array elements
 *             -----------------------------------------------------
 *       i    |  real 2*i   |  imag 2*i  | real 2*i+1  | imag 2*i+1 | 
 *             -----------------------------------------------------
 *
 */

void fft_1d_r2(vector float *out, vector float *in, vector float *W, int log2_size);

#endif /* INCLUDED_LIBFFT_H */
