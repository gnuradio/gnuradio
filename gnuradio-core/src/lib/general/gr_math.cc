/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gr_math.h>
#include <math.h>

/*
 * Greatest Common Divisor, using Euclid's algorithm.
 * [There are faster algorithms.  See Knuth 4.5.2 if you care]
 */

long
gr_gcd (long m, long n)
{
  if (m < 0)
    m = -m;
  
  if (n < 0)
    n = -n;

  while (n != 0){
    long	t = m % n;
    m = n;
    n = t;
  }

  return m;
}


/*
 * These really need some configure hacking to figure out the right answer.
 * As a stop gap, try for a macro, and if not that, then try std::
 */

// returns a non-zero value if value is "not-a-number" (NaN), and 0 otherwise

#if defined(isnan) || !defined(CXX_HAS_STD_ISNAN)

int 
gr_isnan (double value)
{
  return isnan (value);
}

#else

int 
gr_isnan (double value)
{
  return std::isnan (value);
}

#endif

// returns a non-zero value if the value of x has its sign bit set.
//
// This  is  not  the  same  as `x < 0.0', because IEEE 754 floating point
// allows zero to be signed.  The comparison `-0.0 < 0.0'  is  false,  but
// `gr_signbit (-0.0)' will return a nonzero value.

#ifdef signbit

int 
gr_signbit (double x)
{
  return signbit (x);
}

#else

int 
gr_signbit (double x)
{
  return std::signbit (x);
}


#endif
