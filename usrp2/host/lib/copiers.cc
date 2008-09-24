/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <usrp2/copiers.h>
#include <gruel/inet.h>
#include <gr_math.h>
#include <math.h>
#include <stdexcept>
#include <assert.h>
#include <string.h>

// FIXME need gruel::not_implemented

namespace usrp2 {

  /*
   * N.B., in all of these, uint32_t *items is NOT 32-bit aligned!
   * FIXME Needs fix for non-x86 machines.
   */

  /*
   * ----------------------------------------------------------------
   * Copy and convert from USRP2 wire format to host format
   * ----------------------------------------------------------------
   */
  void 
  copy_u2_16sc_to_host_16sc(size_t nitems,
			    const uint32_t *items,
			    std::complex<int16_t> *host_items)
  {
#ifdef WORDS_BIGENDIAN

    assert(sizeof(items[0]) == sizeof(host_items[0]));
    memcpy(host_items, items, nitems * sizeof(items[0]));

#else

    // FIXME SIMD welcome here

    for (size_t i = 0; i < nitems; i++){
      uint32_t t = ntohx(items[i]);
      //printf("%9d\n", items[i]);
      host_items[i] = std::complex<int16_t>((t >> 16), t & 0xffff);
    }

#endif
  }


  /*
   * endian swap if required and map [-32768, 32767] -> [1.0, +1.0)
   */
  void 
  copy_u2_16sc_to_host_32fc(size_t nitems,
			    const uint32_t *items,
			    std::complex<float> *host_items)
  {
    for (size_t i = 0; i < nitems; i++){
      uint32_t t = ntohx(items[i]);
      int16_t re = (t >> 16) & 0xffff;
      int16_t im = (t & 0xffff);
      host_items[i] = std::complex<float>(re * 1.0/32768, im * 1.0/32768);
    }
  }

  /*
   * ----------------------------------------------------------------
   * Copy and convert from host format to USRP2 wire format
   * ----------------------------------------------------------------
   */
  void 
  copy_host_16sc_to_u2_16sc(size_t nitems,
			    const std::complex<int16_t> *host_items,
			    uint32_t *items)
  {
#ifdef WORDS_BIGENDIAN

    assert(sizeof(items[0]) == sizeof(host_items[0]));
    memcpy(items, host_items, nitems * sizeof(items[0]));

#else

    // FIXME SIMD welcome here

    for (size_t i = 0; i < nitems; i++){
      items[i] = htonl((host_items[i].real() << 16) | (host_items[i].imag() & 0xffff));
    }

#endif
  }


  static inline int16_t
  clip_and_scale(float x)
  {
    return static_cast<int16_t>(rintf(gr_branchless_clip(x, 1.0) * 32767.0));
  }

  void 
  copy_host_32fc_to_u2_16sc(size_t nitems,
			    const std::complex<float> *host_items,
			    uint32_t *items)
  {
    for (size_t i = 0; i < nitems; i++){
      int16_t re = clip_and_scale(host_items[i].real());
      int16_t im = clip_and_scale(host_items[i].imag());
      
      items[i] = htonl((re << 16) | (im & 0xffff));
    }
  }

}
