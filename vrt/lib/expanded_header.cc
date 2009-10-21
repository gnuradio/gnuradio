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
#include <vrt/expanded_header.h>
#include <gruel/inet.h>
//#include <stdio.h>

namespace vrt {

  // lookup tables indexed by packet type
  unsigned char expanded_header::s_if_data[16] = {
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  unsigned char expanded_header::s_ext_data[16] = {
    0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  unsigned char expanded_header::s_data[16] = {
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  unsigned char expanded_header::s_context[16] = {
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  unsigned char expanded_header::s_stream_id[16] = {
    0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };


  // dispatch codeword bits
  static const int HAS_STREAM_ID       = 1 << 0;
  static const int HAS_CLASS_ID        = 1 << 1;
  static const int HAS_INTEGER_SECS    = 1 << 2;
  static const int HAS_FRACTIONAL_SECS = 1 << 3;
  static const int HAS_TRAILER         = 1 << 4;

#include "expanded_header_cw_tables.h"

  static int
  compute_codeword(const expanded_header &h)
  {
    int cw = 0;
    if (h.stream_id_p())       cw |= HAS_STREAM_ID;
    if (h.class_id_p())        cw |= HAS_CLASS_ID;
    if (h.integer_secs_p())    cw |= HAS_INTEGER_SECS;
    if (h.fractional_secs_p()) cw |= HAS_FRACTIONAL_SECS;
    if (h.trailer_p())         cw |= HAS_TRAILER;
    return cw;
  }

  bool 
  expanded_header::parse(const uint32_t *packet,	// in
			size_t n32_bit_words_packet,	// in
			expanded_header *h,		// out
			const uint32_t **payload,	// out
			size_t *n32_bit_words_payload)	// out
  {
    size_t len = n32_bit_words_packet;
    const uint32_t *p = packet;

    *payload = 0;
    *n32_bit_words_payload = 0;

    // printf("parse: n32_bit_words_packet = %zd\n", n32_bit_words_packet);

    if (len < 1){		// must have at least the header word
      h->header = 0;
      return false;
    }

    h->header = ntohl(p[0]);

    if (h->pkt_size() > len)
      return false;		// VRT header says packet is bigger than what we've got

    len = h->pkt_size();	// valid length of packet

    int cw = compute_codeword(*h);
    if (cw_header_len(cw) + cw_trailer_len(cw) > len)
      return false;		// negative payload len

    *payload = p + cw_header_len(cw);
    *n32_bit_words_payload = len - (cw_header_len(cw) + cw_trailer_len(cw));

    // printf("parse: hdr = 0x%08x, cw = 0x%02x, cw_header_len(cw) = %d, cw_trailer_len(cw) = %d\n",
    //   h->header, cw, cw_header_len(cw), cw_trailer_len(cw));

    switch (cw & 0x1f){
#include "expanded_header_switch_body.h"
    }

    /* is this a if context packet? */
    if (h->if_context_p()){
        *payload = p;
        *n32_bit_words_payload = n32_bit_words_packet;
    }

    return true;
  }


}; // vrt
