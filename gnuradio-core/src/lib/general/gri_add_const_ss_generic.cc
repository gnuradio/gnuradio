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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gri_add_const_ss.h>

void
gri_add_const_ss (short *dst, const short *src, int nshorts, short konst)
{
  static const int STRIDE = 8;

  int i;

  for (i = 0; i < nshorts - (STRIDE - 1); i += STRIDE){
    dst[i + 0] = src[i + 0] + konst;
    dst[i + 1] = src[i + 1] + konst;
    dst[i + 2] = src[i + 2] + konst;
    dst[i + 3] = src[i + 3] + konst;
    dst[i + 4] = src[i + 4] + konst;
    dst[i + 5] = src[i + 5] + konst;
    dst[i + 6] = src[i + 6] + konst;
    dst[i + 7] = src[i + 7] + konst;
  }

  for (; i < nshorts; i++)
    dst[i] = src[i] + konst;
}
