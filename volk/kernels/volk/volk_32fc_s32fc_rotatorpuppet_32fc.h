/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H


#include <volk/volk_complex.h>
#include <stdio.h>
#include <volk/volk_32fc_s32fc_x2_rotator_32fc.h>


#ifdef LV_HAVE_GENERIC

/*!
  \brief rotate input vector at fixed rate per sample from initial phase offset
  \param outVector The vector where the results will be stored
  \param inVector Vector to be rotated
  \param phase_inc rotational velocity
  \param phase initial phase offset
  \param num_points The number of values in inVector to be rotated and stored into cVector
*/
static inline void volk_32fc_s32fc_rotatorpuppet_32fc_generic(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){
    lv_32fc_t phase[1] = {lv_cmake(.3, 0.95393)};
    volk_32fc_s32fc_x2_rotator_32fc_generic(outVector, inVector, phase_inc, phase, num_points);

}

#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void volk_32fc_s32fc_rotatorpuppet_32fc_a_sse4_1(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_a_sse4_1(outVector, inVector, phase_inc, phase, num_points);

}

#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
static inline void volk_32fc_s32fc_rotatorpuppet_32fc_u_sse4_1(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_u_sse4_1(outVector, inVector, phase_inc, phase, num_points);

}

#endif /* LV_HAVE_SSE4_1 */


#ifdef LV_HAVE_AVX
#include <immintrin.h>

/*!
  \brief rotate input vector at fixed rate per sample from initial phase offset
  \param outVector The vector where the results will be stored
  \param inVector Vector to be rotated
  \param phase_inc rotational velocity
  \param phase initial phase offset
  \param num_points The number of values in inVector to be rotated and stored into cVector
*/
static inline void volk_32fc_s32fc_rotatorpuppet_32fc_a_avx(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_a_avx(outVector, inVector, phase_inc, phase, num_points);
}

#endif /* LV_HAVE_AVX */


#ifdef LV_HAVE_AVX
#include <immintrin.h>

static inline void volk_32fc_s32fc_rotatorpuppet_32fc_u_avx(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_u_avx(outVector, inVector, phase_inc, phase, num_points);
}

#endif /* LV_HAVE_AVX */

#endif /* INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H */
