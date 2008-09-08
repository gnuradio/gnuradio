/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#ifndef INCLUDED_USRP2_BYTESEX_H
#define INCLUDED_USRP2_BYTESEX_H

// The USRP2 speaks big-endian...
// Use the standard include files or provide substitutions for
// htons and friends

#if defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#elif defined(HAVE_NETINET_IN_H)
#include <netinet/in.h>
#else
#include <stdint.h>

#ifdef WORDS_BIGENDIAN  // nothing to do...

static inline uint32_t htonl(uint32_t x){ return x; }
static inline uint16_t htons(uint16_t x){ return x; }
static inline uint32_t ntohl(uint32_t x){ return x; }
static inline uint16_t ntohs(uint16_t x){ return x; }

#else

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#else

static inline uint16_t
bswap_16 (uint16_t x)
{
  return ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8));
}

static inline uint32_t
bswap_32 (uint32_t x)
{
  return ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) \
        | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24));
}
#endif

static inline uint32_t htonl(uint32_t x){ return bswap_32(x); }
static inline uint16_t htons(uint16_t x){ return bswap_16(x); }
static inline uint32_t ntohl(uint32_t x){ return bswap_32(x); }
static inline uint16_t ntohs(uint16_t x){ return bswap_16(x); }

#endif
#endif
#endif /* INCLUDED_USRP2_BYTESEX_H */
