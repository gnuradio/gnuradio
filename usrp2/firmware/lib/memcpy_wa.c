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

#include "memcpy_wa.h"
#include <stdint.h>
#include <stdlib.h>

/*
 * For copying to/from non-byte-adressable memory, such as
 * the buffers.  dst, src, and nbytes must all satisfy (x % 4 == 0)
 */
void
memcpy_wa(void *dst, const void *src, size_t nbytes)
{
  if (((intptr_t) dst & 0x3)
      || ((intptr_t) src & 0x3)
      || (nbytes & 0x3))
    exit(1);			/* die! */

  int *dp = (int *) dst;
  int *sp = (int *) src;
  unsigned  nw = nbytes/4;

  unsigned i;
  for (i = 0; i < nw; i++)
    dp[i] = sp[i];
}
