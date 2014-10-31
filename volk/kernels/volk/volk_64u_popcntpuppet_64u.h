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

#ifndef INCLUDED_volk_64u_popcntpuppet_64u_H
#define INCLUDED_volk_64u_popcntpuppet_64u_H

#include <stdint.h>
#include <volk/volk_64u_popcnt.h>

#ifdef LV_HAVE_GENERIC
static inline void volk_64u_popcntpuppet_64u_generic(uint64_t* outVector, const uint64_t* inVector, unsigned int num_points){
    unsigned int ii;
    for(ii=0; ii < num_points; ++ii) {
        volk_64u_popcnt_generic(outVector+ii, num_points );
        
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_NEON
static inline void volk_64u_popcntpuppet_64u_neon(uint64_t* outVector, const uint64_t* inVector, unsigned int num_points){
    unsigned int ii;
    for(ii=0; ii < num_points; ++ii) {
        volk_64u_popcnt_neon(outVector+ii, num_points );
    }
}
#endif /* LV_HAVE_NEON */

#endif /* INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H */
