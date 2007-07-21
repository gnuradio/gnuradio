/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
#ifndef INCLUDED_USRP_BYTESEX_H
#define INCLUDED_USRP_BYTESEX_H

/*!
 * \brief routines for convertering between host and usrp byte order
 *
 * Prior to including this file, the user must include "config.h"
 * which will or won't define WORDS_BIGENDIAN based on the
 * result of the AC_C_BIGENDIAN autoconf test.
 */

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#else

#warning Using non-portable code (likely wrong other than ILP32).

static inline unsigned short int
bswap_16 (unsigned short int x)
{
  return ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8));
}

static inline unsigned int
bswap_32 (unsigned int x)
{
  return ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) \
        | (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24));
}
#endif


#ifdef WORDS_BIGENDIAN

static inline unsigned int
host_to_usrp_u32 (unsigned int x)
{
  return bswap_32(x);
}

static inline unsigned int
usrp_to_host_u32 (unsigned int x)
{
  return bswap_32(x);
}

static inline short int
host_to_usrp_short (short int x)
{
  return bswap_16 (x);
}

static inline short int
usrp_to_host_short (short int x)
{
  return bswap_16 (x);
}

#else

static inline unsigned int
host_to_usrp_u32 (unsigned int x)
{
  return x;
}

static inline unsigned int
usrp_to_host_u32 (unsigned int x)
{
  return x;
}

static inline short int
host_to_usrp_short (short int x)
{
  return x;
}

static inline short int
usrp_to_host_short (unsigned short int x)
{
  return x;
}

#endif

#endif /* INCLUDED_USRP_BYTESEX_H */
