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

#ifndef INCLUDED_volk_32u_popcntpuppet_32u_H
#define INCLUDED_volk_32u_popcntpuppet_32u_H

#include <stdint.h>
#include <volk/volk_32u_popcnt.h>

#ifdef LV_HAVE_GENERIC
static inline void volk_32u_popcntpuppet_32u_generic(uint32_t* outVector, const uint32_t* inVector, unsigned int num_points){
    unsigned int ii;
    for(ii=0; ii < num_points; ++ii) {
        volk_32u_popcnt_generic(outVector+ii, *(inVector+ii) );
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_SSE4_2
static inline void volk_32u_popcntpuppet_32u_a_sse4_2(uint32_t* outVector, const uint32_t* inVector, unsigned int num_points){
    unsigned int ii;
    for(ii=0; ii < num_points; ++ii) {
        volk_32u_popcnt_a_sse4_2(outVector+ii, *(inVector+ii) );
    }
}
#endif /* LV_HAVE_SSE4_2 */

#endif /* INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H */
