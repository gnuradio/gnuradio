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
#ifndef INCLUDED_VRT_EXPANDED_HEADER_H
#define INCLUDED_VRT_EXPANDED_HEADER_H

#include <stdint.h>
#include <stddef.h>
#include <vrt/bits.h>

namespace vrt {

  static const size_t HEADER_MAX_N32_BIT_WORDS = 7;
  static const size_t TRAILER_MAX_N32_BIT_WORDS = 1;

  /*!
   * \brief All headers and trailer for VRT IF-Data, Extension-Data,
   * IF-Context and Extension-Context packets.
   *
   * There are fields allocated for each possible header.  Their content may
   * or may not be valid.  Check the header field to confirm their validity.
   * All values are in host-endian format.
   */
  struct expanded_header {
    uint32_t	header;			// first word of all packets
    uint32_t	stream_id;		// optional stream identifier
    uint64_t	class_id;		// optional class identifier
    uint32_t	integer_secs;		// optional integer seconds timestamp
    uint64_t	fractional_secs;	// optional fractional seconds timestamp
    uint32_t	trailer;		// optional trailer (only possible in data pkts)

    expanded_header()
      : header(0) /*, stream_id(0), class_id(0),
        integer_secs(0), fractional_secs(0), trailer(0)*/ {}


    int pkt_type() const {
      return (header & VRTH_PT_MASK) >> 28;
    }
    
    int pkt_cnt() const { return vrth_pkt_cnt(header); }
    size_t pkt_size() const { return vrth_pkt_size(header); }


    // packet type predicates
    bool if_data_p() const { return s_if_data[pkt_type()]; }
    bool ext_data_p() const { return s_ext_data[pkt_type()]; }
    bool if_context_p() const { return (header & VRTH_PT_MASK) == VRTH_PT_IF_CONTEXT; }
    bool ext_context_p() const { return (header & VRTH_PT_MASK) == VRTH_PT_EXT_CONTEXT; }

    bool data_p() const { return s_data[pkt_type()]; }	// if_data_p() || ext_data_p()
    bool context_p() const { return s_context[pkt_type()]; }	// if_context_p() || ext_context_p()

    // optional info predicates
    bool stream_id_p() const { return s_stream_id[pkt_type()]; }
    bool class_id_p() const { return (header & VRTH_HAS_CLASSID) != 0; }
    bool integer_secs_p() const { return (header & VRTH_TSI_MASK) != 0; }
    bool fractional_secs_p() const { return (header & VRTH_TSF_MASK) != 0; }
    bool trailer_p() const { return (header & VRTH_HAS_TRAILER) != 0 && data_p(); }


    /*!
     * \brief unparse expanded header, fill-in the words of a vrt packet header and trailer
     * This method is only intended to fill the buffers with header and trailer information.
     * The actual handling of the separate header, payload, trailer buffers is up to the caller.
     */
    static void unparse(const expanded_header *hdr,    // in
                        size_t n32_bit_words_payload,  // in
                        uint32_t *header,              // out
                        size_t *n32_bit_words_header,  // out
                        uint32_t *trailer,             // out
                        size_t *n32_bit_words_trailer);// out

    /*!
     * \brief parse packet, fill-in expanded header, start of payload and len of payload
     */
    static bool parse(const uint32_t *packet,		// in
		      size_t n32_bit_words_packet,	// in
		      expanded_header *hdr,		// out
		      const uint32_t **payload,		// out
		      size_t *n32_bit_words_payload);	// out
		      
  private:
    static unsigned char s_if_data[16];
    static unsigned char s_ext_data[16];
    static unsigned char s_data[16];
    static unsigned char s_context[16];
    static unsigned char s_stream_id[16];

  };

}; // vrt


#endif /* INCLUDED_VRT_EXPANDED_HEADER_H */
