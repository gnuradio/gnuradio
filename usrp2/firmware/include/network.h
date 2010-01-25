/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

/*!
 * Various networking related structures and defines
 */

#ifndef INCLUDED_NETWORK_H
#define INCLUDED_NETWORK_H

#include <stdint.h>
#include <stdbool.h>

// Ethernet MAC address

typedef struct {
  uint8_t	addr[6];
} eth_mac_addr_t;


// IPv4 address

#ifndef __cplusplus

struct in_addr {
  uint32_t	s_addr;
};

static inline struct in_addr
make_in_addr(uint32_t addr)
{
  struct in_addr r;
  r.s_addr = addr;
  return r;
}

static inline bool
in_addr_eq(const struct in_addr a, const struct in_addr b)
{
  return a.s_addr == b.s_addr;
}

#define MK_IP_ADDR(a, b, c, d) \
  (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))


// IPv4 AF_INET sockets:

struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;
};

static inline struct sockaddr_in 
make_sockaddr_in(struct in_addr addr, int port)
{
  struct sockaddr_in r;
  r.sin_family = 0;		// not used
  r.sin_port = port;
  r.sin_addr = addr;
  return r;
}

#endif

#endif /* INCLUDED_NETWORK_H */
