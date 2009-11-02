/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#include <vrt/copiers.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string.h>

namespace vrt {

  void 
  copy_net_16sc_to_host_16sc(size_t nitems,
			     const uint32_t *items,
			     std::complex<int16_t> *host_items)
  {
#ifdef WORDS_BIGENDIAN

    assert(sizeof(items[0]) == sizeof(host_items[0]));
    memcpy(host_items, items, nitems * sizeof(items[0]));

#else

    // FIXME SIMD welcome here

    for (size_t i = 0; i < nitems; i++){
      uint32_t t = ntohl(items[i]);
      //printf("%9d\n", items[i]);
      host_items[i] = std::complex<int16_t>((t >> 16), t & 0xffff);
    }

#endif
  }

  void
  copy_net_16sc_to_host_32fc(size_t nitems,
			     const uint32_t *items,
			     std::complex<float> *host_items)
  {
    // FIXME SIMD welcome here

    for (size_t i = 0; i < nitems; i++){
      uint32_t t = ntohl(items[i]);
      int16_t re = (t >> 16) & 0xffff;
      int16_t im = (t & 0xffff);
      host_items[i] = std::complex<float>(re * 1.0/32768, im * 1.0/32768);
    }
  }

};

