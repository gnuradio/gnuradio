/* --------------------------------------------------------------  */
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
#ifndef _FFT_1D_R2_H_
#define _FFT_1D_R2_H_	1

#include "fft_1d.h"

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


static __inline void _fft_1d_r2(vector float *out, vector float *in, vector float *W, int log2_size)
{
  int i, j, k;
  int stage, offset;
  int i_rev;
  int n, n_2, n_4, n_8, n_16, n_3_16;
  int w_stride, w_2stride, w_3stride, w_4stride;
  int stride, stride_2, stride_4, stride_3_4;
  vector float *W0, *W1, *W2, *W3;
  vector float *re0, *re1, *re2, *re3;
  vector float *im0, *im1, *im2, *im3;
  vector float *in0, *in1, *in2, *in3, *in4, *in5, *in6, *in7;
  vector float *out0, *out1, *out2, *out3;
  vector float tmp0, tmp1;
  vector float w0_re, w0_im, w1_re, w1_im;
  vector float w0, w1, w2, w3;
  vector float src_lo0, src_lo1, src_lo2, src_lo3;
  vector float src_hi0, src_hi1, src_hi2, src_hi3;
  vector float dst_lo0, dst_lo1, dst_lo2, dst_lo3;
  vector float dst_hi0, dst_hi1, dst_hi2, dst_hi3;
  vector float out_re_lo0, out_re_lo1, out_re_lo2, out_re_lo3;
  vector float out_im_lo0, out_im_lo1, out_im_lo2, out_im_lo3;
  vector float out_re_hi0, out_re_hi1, out_re_hi2, out_re_hi3;
  vector float out_im_hi0, out_im_hi1, out_im_hi2, out_im_hi3;
  vector float re_lo0,  re_lo1,  re_lo2,  re_lo3;
  vector float im_lo0,  im_lo1,  im_lo2,  im_lo3;
  vector float re_hi0,  re_hi1,  re_hi2,  re_hi3;
  vector float im_hi0,  im_hi1,  im_hi2,  im_hi3;
  vector float pq_lo0,  pq_lo1,  pq_lo2,  pq_lo3;
  vector float pq_hi0,  pq_hi1,  pq_hi2,  pq_hi3;
  vector float re[MAX_FFT_1D_SIZE/4], im[MAX_FFT_1D_SIZE/4];	/* real & imaginary working arrays */
  vector float ppmm = (vector float) { 1.0f,  1.0f, -1.0f, -1.0f};
  vector float pmmp = (vector float) { 1.0f, -1.0f, -1.0f,  1.0f};
  vector unsigned char reverse;
  vector unsigned char shuf_lo = (vector unsigned char) {
					     0,  1, 2, 3,  4, 5, 6, 7,
					     16,17,18,19, 20,21,22,23};
  vector unsigned char shuf_hi = (vector unsigned char) {
					     8,  9,10,11, 12,13,14,15,
					     24,25,26,27, 28,29,30,31};
  vector unsigned char shuf_0202 = (vector unsigned char) {
					       0, 1, 2, 3,  8, 9,10,11,
					       0, 1, 2, 3,  8, 9,10,11};
  vector unsigned char shuf_1313 = (vector unsigned char) {
					       4, 5, 6, 7, 12,13,14,15,
					       4, 5, 6, 7, 12,13,14,15};
  vector unsigned char shuf_0303 = (vector unsigned char) { 
					       0, 1, 2, 3, 12,13,14,15,
					       0, 1, 2, 3, 12,13,14,15};
  vector unsigned char shuf_1212 = (vector unsigned char) {
					       4, 5, 6, 7,  8, 9,10,11,
					       4, 5, 6, 7,  8, 9,10,11};
  vector unsigned char shuf_0415 = (vector unsigned char) {
					       0, 1, 2, 3, 16,17,18,19,
					       4, 5, 6, 7, 20,21,22,23};
  vector unsigned char shuf_2637 = (vector unsigned char) {
					       8, 9,10,11, 24,25,26,27,
					       12,13,14,15,28,29,30,31};
  vector unsigned char shuf_0246 = (vector unsigned char) {
					       0, 1, 2, 3,  8, 9,10,11,
					       16,17,18,19,24,25,26,27};
  vector unsigned char shuf_1357 = (vector unsigned char) {
					       4, 5, 6, 7, 12,13,14,15,
					       20,21,22,23,28,29,30,31};
  
  n = 1 << log2_size;
  n_2  = n >> 1;
  n_4  = n >> 2;
  n_8  = n >> 3;
  n_16 = n >> 4;

  n_3_16 = n_8 + n_16;

  /* Compute a byte reverse shuffle pattern to be used to produce
   * an address bit swap.
   */
  reverse = spu_or(spu_slqwbyte(spu_splats((unsigned char)0x80), log2_size),
		   spu_rlmaskqwbyte(((vec_uchar16){15,14,13,12, 11,10,9,8, 
					 	    7, 6, 5, 4,  3, 2,1,0}),
				    log2_size-16));

  /* Perform the first 3 stages of the FFT. These stages differs from 
   * other stages in that the inputs are unscrambled and the data is 
   * reformated into parallel arrays (ie, seperate real and imaginary
   * arrays). The term "unscramble" means the bit address reverse the 
   * data array. In addition, the first three stages have simple twiddle
   * weighting factors.
   *		stage 1: (1, 0)
   *            stage 2: (1, 0) and (0, -1)
   *		stage 3: (1, 0), (0.707, -0.707), (0, -1), (-0.707, -0.707)
   *
   * The arrays are processed as two halves, simultaneously. The lo (first 
   * half) and hi (second half). This is done because the scramble 
   * shares source value between each half of the output arrays.
   */
  i = 0;
  i_rev = 0;

  in0 = in;
  in1 = in + n_8;
  in2 = in + n_16;
  in3 = in + n_3_16;  

  in4 = in  + n_4;
  in5 = in1 + n_4;
  in6 = in2 + n_4;
  in7 = in3 + n_4;

  re0 = re;
  re1 = re + n_8;
  im0 = im;
  im1 = im + n_8;

  w0_re = (vector float) { 1.0f,  INV_SQRT_2,  0.0f, -INV_SQRT_2};
  w0_im = (vector float) { 0.0f, -INV_SQRT_2, -1.0f, -INV_SQRT_2};
      
  do {
    src_lo0 = in0[i_rev];
    src_lo1 = in1[i_rev];
    src_lo2 = in2[i_rev];
    src_lo3 = in3[i_rev];

    src_hi0 = in4[i_rev];
    src_hi1 = in5[i_rev];
    src_hi2 = in6[i_rev];
    src_hi3 = in7[i_rev];

    /* Perform scramble.
     */
    dst_lo0 = spu_shuffle(src_lo0, src_hi0, shuf_lo);
    dst_hi0 = spu_shuffle(src_lo0, src_hi0, shuf_hi);
    dst_lo1 = spu_shuffle(src_lo1, src_hi1, shuf_lo);
    dst_hi1 = spu_shuffle(src_lo1, src_hi1, shuf_hi);
    dst_lo2 = spu_shuffle(src_lo2, src_hi2, shuf_lo);
    dst_hi2 = spu_shuffle(src_lo2, src_hi2, shuf_hi);
    dst_lo3 = spu_shuffle(src_lo3, src_hi3, shuf_lo);
    dst_hi3 = spu_shuffle(src_lo3, src_hi3, shuf_hi);

    /* Perform the stage 1 butterfly. The multiplier constant, ppmm,
     * is used to control the sign of the operands since a single
     * quadword contains both of P and Q valule of the butterfly.
     */
    pq_lo0 = spu_madd(ppmm, dst_lo0, spu_rlqwbyte(dst_lo0, 8));
    pq_hi0 = spu_madd(ppmm, dst_hi0, spu_rlqwbyte(dst_hi0, 8));
    pq_lo1 = spu_madd(ppmm, dst_lo1, spu_rlqwbyte(dst_lo1, 8));
    pq_hi1 = spu_madd(ppmm, dst_hi1, spu_rlqwbyte(dst_hi1, 8));
    pq_lo2 = spu_madd(ppmm, dst_lo2, spu_rlqwbyte(dst_lo2, 8));
    pq_hi2 = spu_madd(ppmm, dst_hi2, spu_rlqwbyte(dst_hi2, 8));
    pq_lo3 = spu_madd(ppmm, dst_lo3, spu_rlqwbyte(dst_lo3, 8));
    pq_hi3 = spu_madd(ppmm, dst_hi3, spu_rlqwbyte(dst_hi3, 8));

    /* Perfrom the stage 2 butterfly. For this stage, the 
     * inputs pq are still interleaved (p.real, p.imag, q.real, 
     * q.imag), so we must first re-order the data into 
     * parallel arrays as well as perform the reorder 
     * associated with the twiddle W[n/4], which equals
     * (0, -1). 
     *
     *	ie. (A, B) * (0, -1) => (B, -A)
     */
    re_lo0 = spu_madd(ppmm, 
		      spu_shuffle(pq_lo1, pq_lo1, shuf_0303),
		      spu_shuffle(pq_lo0, pq_lo0, shuf_0202));
    im_lo0 = spu_madd(pmmp, 
		      spu_shuffle(pq_lo1, pq_lo1, shuf_1212),
		      spu_shuffle(pq_lo0, pq_lo0, shuf_1313));

    re_lo1 = spu_madd(ppmm, 
		      spu_shuffle(pq_lo3, pq_lo3, shuf_0303),
		      spu_shuffle(pq_lo2, pq_lo2, shuf_0202));
    im_lo1 = spu_madd(pmmp, 
		      spu_shuffle(pq_lo3, pq_lo3, shuf_1212),
		      spu_shuffle(pq_lo2, pq_lo2, shuf_1313));


    re_hi0 = spu_madd(ppmm, 
		      spu_shuffle(pq_hi1, pq_hi1, shuf_0303),
		      spu_shuffle(pq_hi0, pq_hi0, shuf_0202));
    im_hi0 = spu_madd(pmmp, 
		       spu_shuffle(pq_hi1, pq_hi1, shuf_1212),
		       spu_shuffle(pq_hi0, pq_hi0, shuf_1313));

    re_hi1 = spu_madd(ppmm, 
		      spu_shuffle(pq_hi3, pq_hi3, shuf_0303),
		      spu_shuffle(pq_hi2, pq_hi2, shuf_0202));
    im_hi1 = spu_madd(pmmp, 
		      spu_shuffle(pq_hi3, pq_hi3, shuf_1212),
		      spu_shuffle(pq_hi2, pq_hi2, shuf_1313));


    /* Perform stage 3 butterfly.
     */
    FFT_1D_BUTTERFLY(re0[0], im0[0], re0[1], im0[1], re_lo0, im_lo0, re_lo1, im_lo1, w0_re, w0_im);
    FFT_1D_BUTTERFLY(re1[0], im1[0], re1[1], im1[1], re_hi0, im_hi0, re_hi1, im_hi1, w0_re, w0_im);

    re0 += 2;
    re1 += 2;
    im0 += 2; 
    im1 += 2;
    
    i += 8;
    i_rev = BIT_SWAP(i, reverse) / 2;
  } while (i < n_2);

  /* Process stages 4 to log2_size-2
   */
  for (stage=4, stride=4; stage<log2_size-1; stage++, stride += stride) {
    w_stride  = n_2 >> stage;
    w_2stride = n   >> stage;
    w_3stride = w_stride +  w_2stride;
    w_4stride = w_2stride + w_2stride;

    W0 = W;
    W1 = W + w_stride;
    W2 = W + w_2stride;
    W3 = W + w_3stride;

    stride_2 = stride >> 1;
    stride_4 = stride >> 2;
    stride_3_4 = stride_2 + stride_4;

    re0 = re;              im0 = im;
    re1 = re + stride_2;   im1 = im + stride_2;   
    re2 = re + stride_4;   im2 = im + stride_4;   
    re3 = re + stride_3_4; im3 = im + stride_3_4;   

    for (i=0, offset=0; i<stride_4; i++, offset += w_4stride) {
      /* Compute the twiddle factors
       */
      w0 = W0[offset];
      w1 = W1[offset];
      w2 = W2[offset];
      w3 = W3[offset];

      tmp0 = spu_shuffle(w0, w2, shuf_0415);
      tmp1 = spu_shuffle(w1, w3, shuf_0415);

      w0_re = spu_shuffle(tmp0, tmp1, shuf_0415);
      w0_im = spu_shuffle(tmp0, tmp1, shuf_2637);

      j = i;
      k = i + stride;
      do {
	re_lo0 = re0[j]; im_lo0 = im0[j];
	re_lo1 = re1[j]; im_lo1 = im1[j];

	re_hi0 = re2[j]; im_hi0 = im2[j];
	re_hi1 = re3[j]; im_hi1 = im3[j];

	re_lo2 = re0[k]; im_lo2 = im0[k];
	re_lo3 = re1[k]; im_lo3 = im1[k];

	re_hi2 = re2[k]; im_hi2 = im2[k];
	re_hi3 = re3[k]; im_hi3 = im3[k];

	FFT_1D_BUTTERFLY   (re0[j], im0[j], re1[j], im1[j], re_lo0, im_lo0, re_lo1, im_lo1, w0_re, w0_im);
	FFT_1D_BUTTERFLY_HI(re2[j], im2[j], re3[j], im3[j], re_hi0, im_hi0, re_hi1, im_hi1, w0_re, w0_im);

	FFT_1D_BUTTERFLY   (re0[k], im0[k], re1[k], im1[k], re_lo2, im_lo2, re_lo3, im_lo3, w0_re, w0_im);
	FFT_1D_BUTTERFLY_HI(re2[k], im2[k], re3[k], im3[k], re_hi2, im_hi2, re_hi3, im_hi3, w0_re, w0_im);

	j += 2 * stride;
	k += 2 * stride;
      } while (j < n_4);
    }
  }

  /* Process stage log2_size-1. This is identical to the stage processing above
   * except for this stage the inner loop is only executed once so it is removed
   * entirely.
   */
  w_stride  = n_2 >> stage;
  w_2stride = n   >> stage;
  w_3stride = w_stride +  w_2stride;
  w_4stride = w_2stride + w_2stride;

  stride_2 = stride >> 1;
  stride_4 = stride >> 2;

  stride_3_4 = stride_2 + stride_4;

  re0 = re;              im0 = im;
  re1 = re + stride_2;   im1 = im + stride_2;   
  re2 = re + stride_4;   im2 = im + stride_4;   
  re3 = re + stride_3_4; im3 = im + stride_3_4;   

  for (i=0, offset=0; i<stride_4; i++, offset += w_4stride) {
    /* Compute the twiddle factors
     */
    w0 = W[offset];
    w1 = W[offset + w_stride];
    w2 = W[offset + w_2stride];
    w3 = W[offset + w_3stride];

    tmp0 = spu_shuffle(w0, w2, shuf_0415);
    tmp1 = spu_shuffle(w1, w3, shuf_0415);

    w0_re = spu_shuffle(tmp0, tmp1, shuf_0415);
    w0_im = spu_shuffle(tmp0, tmp1, shuf_2637);

    j = i;
    k = i + stride;

    re_lo0 = re0[j]; im_lo0 = im0[j];
    re_lo1 = re1[j]; im_lo1 = im1[j];

    re_hi0 = re2[j]; im_hi0 = im2[j];
    re_hi1 = re3[j]; im_hi1 = im3[j];

    re_lo2 = re0[k]; im_lo2 = im0[k];
    re_lo3 = re1[k]; im_lo3 = im1[k];

    re_hi2 = re2[k]; im_hi2 = im2[k];
    re_hi3 = re3[k]; im_hi3 = im3[k];
      
    FFT_1D_BUTTERFLY   (re0[j], im0[j], re1[j], im1[j], re_lo0, im_lo0, re_lo1, im_lo1, w0_re, w0_im);
    FFT_1D_BUTTERFLY_HI(re2[j], im2[j], re3[j], im3[j], re_hi0, im_hi0, re_hi1, im_hi1, w0_re, w0_im);

    FFT_1D_BUTTERFLY   (re0[k], im0[k], re1[k], im1[k], re_lo2, im_lo2, re_lo3, im_lo3, w0_re, w0_im);
    FFT_1D_BUTTERFLY_HI(re2[k], im2[k], re3[k], im3[k], re_hi2, im_hi2, re_hi3, im_hi3, w0_re, w0_im);
  }


  /* Process the final stage (stage log2_size). For this stage, 
   * reformat the data from parallel arrays back into 
   * interleaved arrays,storing the result into <in>.
   *
   * This loop has been manually unrolled by 2 to improve 
   * dual issue rates and reduce stalls. This unrolling
   * forces a minimum FFT size of 32.
   */
  re0 = re;
  re1 = re + n_8;
  re2 = re + n_16;
  re3 = re + n_3_16;

  im0 = im;
  im1 = im + n_8;
  im2 = im + n_16;
  im3 = im + n_3_16;

  out0 = out;
  out1 = out + n_4;
  out2 = out + n_8;
  out3 = out1 + n_8;

  i = n_16;

  do {
    /* Fetch the twiddle factors
     */
    w0 = W[0];
    w1 = W[1];
    w2 = W[2];
    w3 = W[3];

    W += 4;

    w0_re = spu_shuffle(w0, w1, shuf_0246);
    w0_im = spu_shuffle(w0, w1, shuf_1357);
    w1_re = spu_shuffle(w2, w3, shuf_0246);
    w1_im = spu_shuffle(w2, w3, shuf_1357);

    /* Fetch the butterfly inputs, reals and imaginaries
     */
    re_lo0 = re0[0]; im_lo0 = im0[0];
    re_lo1 = re1[0]; im_lo1 = im1[0];
    re_lo2 = re0[1]; im_lo2 = im0[1];
    re_lo3 = re1[1]; im_lo3 = im1[1];

    re_hi0 = re2[0]; im_hi0 = im2[0];
    re_hi1 = re3[0]; im_hi1 = im3[0];
    re_hi2 = re2[1]; im_hi2 = im2[1];
    re_hi3 = re3[1]; im_hi3 = im3[1];

    re0 += 2; im0 += 2;
    re1 += 2; im1 += 2;
    re2 += 2; im2 += 2;
    re3 += 2; im3 += 2;

    /* Perform the butterflys
     */
    FFT_1D_BUTTERFLY   (out_re_lo0, out_im_lo0, out_re_lo1, out_im_lo1, re_lo0, im_lo0, re_lo1, im_lo1, w0_re, w0_im);
    FFT_1D_BUTTERFLY   (out_re_lo2, out_im_lo2, out_re_lo3, out_im_lo3, re_lo2, im_lo2, re_lo3, im_lo3, w1_re, w1_im);

    FFT_1D_BUTTERFLY_HI(out_re_hi0, out_im_hi0, out_re_hi1, out_im_hi1, re_hi0, im_hi0, re_hi1, im_hi1, w0_re, w0_im);
    FFT_1D_BUTTERFLY_HI(out_re_hi2, out_im_hi2, out_re_hi3, out_im_hi3, re_hi2, im_hi2, re_hi3, im_hi3, w1_re, w1_im);

    /* Interleave the results and store them into the output buffers (ie,
     * the original input buffers.
     */
    out0[0] = spu_shuffle(out_re_lo0, out_im_lo0, shuf_0415);
    out0[1] = spu_shuffle(out_re_lo0, out_im_lo0, shuf_2637);
    out0[2] = spu_shuffle(out_re_lo2, out_im_lo2, shuf_0415);
    out0[3] = spu_shuffle(out_re_lo2, out_im_lo2, shuf_2637);

    out1[0] = spu_shuffle(out_re_lo1, out_im_lo1, shuf_0415);
    out1[1] = spu_shuffle(out_re_lo1, out_im_lo1, shuf_2637);
    out1[2] = spu_shuffle(out_re_lo3, out_im_lo3, shuf_0415);
    out1[3] = spu_shuffle(out_re_lo3, out_im_lo3, shuf_2637);

    out2[0] = spu_shuffle(out_re_hi0, out_im_hi0, shuf_0415);
    out2[1] = spu_shuffle(out_re_hi0, out_im_hi0, shuf_2637);
    out2[2] = spu_shuffle(out_re_hi2, out_im_hi2, shuf_0415);
    out2[3] = spu_shuffle(out_re_hi2, out_im_hi2, shuf_2637);

    out3[0] = spu_shuffle(out_re_hi1, out_im_hi1, shuf_0415);
    out3[1] = spu_shuffle(out_re_hi1, out_im_hi1, shuf_2637);
    out3[2] = spu_shuffle(out_re_hi3, out_im_hi3, shuf_0415);
    out3[3] = spu_shuffle(out_re_hi3, out_im_hi3, shuf_2637);

    out0 += 4;
    out1 += 4;
    out2 += 4;
    out3 += 4;

    i -= 2;
  } while (i);
}

#endif /* _FFT_1D_R2_H_ */
