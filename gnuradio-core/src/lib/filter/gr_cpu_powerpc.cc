/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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

#include <gr_cpu.h>

bool
gr_cpu::has_mmx ()
{
  return false;
}

bool
gr_cpu::has_sse ()
{
  return false;
}

bool
gr_cpu::has_sse2 ()
{
  return false;
}

bool
gr_cpu::has_3dnow ()
{
  return false;
}

bool
gr_cpu::has_3dnowext ()
{
  return false;
}

bool
gr_cpu::has_altivec ()
{
  return true;		// FIXME assume we've always got it
}

bool
gr_cpu::has_armv7_a ()
{
  return false;
}
