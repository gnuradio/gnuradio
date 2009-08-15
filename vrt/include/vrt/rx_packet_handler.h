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
#ifndef INCLUDED_VRT_RX_PACKET_HANDLER_H
#define INCLUDED_VRT_RX_PACKET_HANDLER_H

#include <vrt/expanded_header.h>
#include <stddef.h>

namespace vrt {

  /*!
   * \brief Abstract function object called to handle received VRT packets.
   *
   * An object derived from this class is passed to vrt_rx_udp::rx_packets
   * to process the received packets.
   */
  class rx_packet_handler {
  public:
    virtual ~rx_packet_handler();

    /*!
     * \param payload points to the first 32-bit word of the payload field.
     * \param n32_bit_words is the number of 32-bit words in the payload field.
     * \param hdr is the expanded version of the mandatory and optional header fields (& trailer).
     *
     * \p payload points to the raw payload section of the packet received off
     * the wire. The data is network-endian (aka big-endian) 32-bit integers.
     *
     * This is the general purpose, low level interface and relies on other
     * functions to handle all required endian-swapping and format conversion
     * of the payload.  \sa FIXME.
     *
     * \returns true if the object wants to be called again with new data;
     * false if no additional data is wanted.
     */
    virtual bool operator()(const uint32_t *payload,
			    size_t n32_bit_words,
			    const expanded_header *hdr);
  };

};  // vrt


#endif /* INCLUDED_VRT_RX_PACKET_HANDLER_H */
