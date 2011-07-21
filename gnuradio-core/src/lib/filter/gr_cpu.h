/* -*- c++ -*- */
/*
 * Copyright 2002,2008,2009 Free Software Foundation, Inc.
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

#ifndef _GR_CPU_H_
#define _GR_CPU_H_

#include <gr_core_api.h>

struct GR_CORE_API gr_cpu {
  static bool has_mmx ();
  static bool has_sse ();
  static bool has_sse2 ();
  static bool has_sse3 ();
  static bool has_ssse3 ();
  static bool has_sse4_1 ();
  static bool has_sse4_2 ();
  static bool has_3dnow ();
  static bool has_3dnowext ();
  static bool has_altivec ();
  static bool has_armv7_a ();
};

#endif /* _GR_CPU_H_ */