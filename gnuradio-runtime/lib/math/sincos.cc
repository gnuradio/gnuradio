/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE		// ask for GNU extensions if available
#endif

#include <gnuradio/sincos.h>
#include <math.h>

namespace gr {

#if defined (HAVE_SINCOS)

  void
  sincos(double x, double *sinx, double *cosx)
  {
    ::sincos(x, sinx, cosx);
  }

#else

  void
  sincos(double x, double *sinx, double *cosx)
  {
    *sinx = ::sin(x);
    *cosx = ::cos(x);
  }

#endif

  // ----------------------------------------------------------------

#if defined (HAVE_SINCOSF)

  void
  sincosf(float x, float *sinx, float *cosx)
  {
    ::sincosf(x, sinx, cosx);
  }

#elif defined (HAVE_SINF) && defined (HAVE_COSF)

  void
  sincosf(float x, float *sinx, float *cosx)
  {
    *sinx = ::sinf(x);
    *cosx = ::cosf(x);
  }

#else

  void
  sincosf(float x, float *sinx, float *cosx)
  {
    *sinx = ::sin(x);
    *cosx = ::cos(x);
  }

#endif

} /* namespace gr */
