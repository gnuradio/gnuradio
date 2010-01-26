/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_USRP2_IPV4_PACKET_H
#define INCLUDED_USRP2_IPV4_PACKET_H

#include "usrp2_cdefs.h"
#include "network.h"

__U2_BEGIN_DECLS

/*!
 * \brief The classic ipv4 header
 */
typedef struct {
  unsigned int   ip_v:4; /* both fields are 4 bits */
  unsigned int   ip_hl:4;
  uint8_t        ip_tos;
  uint16_t       ip_len;
  uint16_t       ip_id;
  uint16_t       ip_off;
  uint8_t        ip_ttl;
  uint8_t        ip_p;
  uint16_t       ip_sum;
  struct in_addr ip_src;
  struct in_addr ip_dst;
} __attribute__((packed)) u2_ipv4_hdr_t;

#define IP_RF 0x8000        /* reserved fragment flag */
#define IP_DF 0x4000        /* dont fragment flag */
#define IP_MF 0x2000        /* more fragments flag */
#define IP_OFFMASK 0x1fff   /* mask for fragmenting bits */

#define IP_PROTO_UDP 17

__U2_END_DECLS

#endif /* INCLUDED_USRP2_IPV4_PACKET_H */
