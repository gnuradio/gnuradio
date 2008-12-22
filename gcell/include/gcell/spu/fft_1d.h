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
#ifndef _FFT_1D_H_
#define _FFT_1D_H_	1

#include <spu_intrinsics.h>

/* BIT_SWAP - swaps up to 16 bits of the integer _i according to the 
 *            pattern specified by _pat.
 */
#define BIT_SWAP(_i, _pat)	  spu_extract(spu_gather(spu_shuffle(spu_maskb(_i), _pat, _pat)), 0)


#ifndef MAX_FFT_1D_SIZE
#define MAX_FFT_1D_SIZE	8192
#endif

#ifndef INV_SQRT_2
#define INV_SQRT_2	0.7071067811865
#endif


/* The following macro, FFT_1D_BUTTERFLY, performs a 4 way SIMD basic butterfly 
 * operation. The inputs are in parallel arrays (seperate real and imaginary
 * vectors). 
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
 */

#define FFT_1D_BUTTERFLY(_P_re, _P_im, _Q_re, _Q_im, _p_re, _p_im, _q_re, _q_im, _W_re, _W_im) {	\
  vector float _qw_re, _qw_im;										\
													\
  _qw_re = spu_msub(_q_re, _W_re, spu_mul(_q_im, _W_im));						\
  _qw_im = spu_madd(_q_re, _W_im, spu_mul(_q_im, _W_re));						\
  _P_re  = spu_add(_p_re, _qw_re);									\
  _P_im  = spu_add(_p_im, _qw_im);									\
  _Q_re  = spu_sub(_p_re, _qw_re);									\
  _Q_im  = spu_sub(_p_im, _qw_im);									\
}


/* FFT_1D_BUTTERFLY_HI is equivalent to FFT_1D_BUTTERFLY with twiddle factors (W_im, -W_re)
 */
#define FFT_1D_BUTTERFLY_HI(_P_re, _P_im, _Q_re, _Q_im, _p_re, _p_im, _q_re, _q_im, _W_re, _W_im) {	\
  vector float _qw_re, _qw_im;										\
													\
  _qw_re = spu_madd(_q_re, _W_im, spu_mul(_q_im, _W_re));						\
  _qw_im = spu_msub(_q_im, _W_im, spu_mul(_q_re, _W_re));						\
  _P_re  = spu_add(_p_re, _qw_re);									\
  _P_im  = spu_add(_p_im, _qw_im);									\
  _Q_re  = spu_sub(_p_re, _qw_re);									\
  _Q_im  = spu_sub(_p_im, _qw_im);									\
}

#endif /* _FFT_1D_H_ */
