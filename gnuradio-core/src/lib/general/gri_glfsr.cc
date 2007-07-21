/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <gri_glfsr.h>
#include <stdexcept>

static int s_polynomial_masks[] = {
  0x00000000,
  0x00000001,			// x^1 + 1
  0x00000003,                   // x^2 + x^1 + 1
  0x00000005,                   // x^3 + x^1 + 1
  0x00000009,			// x^4 + x^1 + 1
  0x00000012,			// x^5 + x^2 + 1
  0x00000021,			// x^6 + x^1 + 1
  0x00000041,                   // x^7 + x^1 + 1
  0x0000008E,			// x^8 + x^4 + x^3 + x^2 + 1
  0x00000108,			// x^9 + x^4 + 1
  0x00000204,			// x^10 + x^4 + 1
  0x00000402,			// x^11 + x^2 + 1
  0x00000829,			// x^12 + x^6 + x^4 + x^1 + 1
  0x0000100D,			// x^13 + x^4 + x^3 + x^1 + 1
  0x00002015,			// x^14 + x^5 + x^3 + x^1 + 1
  0x00004001,			// x^15 + x^1 + 1
  0x00008016,			// x^16 + x^5 + x^3 + x^2 + 1
  0x00010004,			// x^17 + x^3 + 1
  0x00020013,			// x^18 + x^5 + x^2 + x^1 + 1
  0x00040013,			// x^19 + x^5 + x^2 + x^1 + 1
  0x00080004,			// x^20 + x^3 + 1
  0x00100002,			// x^21 + x^2 + 1
  0x00200001,			// x^22 + x^1 + 1
  0x00400010,			// x^23 + x^5 + 1
  0x0080000D,			// x^24 + x^4 + x^3 + x^1 + 1
  0x01000004,			// x^25 + x^3 + 1
  0x02000023,			// x^26 + x^6 + x^2 + x^1 + 1
  0x04000013,			// x^27 + x^5 + x^2 + x^1 + 1
  0x08000004,			// x^28 + x^3 + 1
  0x10000002,			// x^29 + x^2 + 1
  0x20000029,			// x^30 + x^4 + x^1 + 1
  0x40000004,			// x^31 + x^3 + 1
  0x80000057			// x^32 + x^7 + x^5 + x^3 + x^2 + x^1 + 1
};

int gri_glfsr::glfsr_mask(int degree)
{
  if (degree < 1 || degree > 32)
    throw std::runtime_error("gri_glfsr::glfsr_mask(): degree must be between 1 and 32 inclusive");
  return s_polynomial_masks[degree];
}
