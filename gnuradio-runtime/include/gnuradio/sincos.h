/* -*- c++ -*- */
/*
 * Copyright 2002,2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SINCOS_H
#define INCLUDED_GR_SINCOS_H

#include <gnuradio/api.h>
#include <cmath>

namespace gr {

#if defined(HAVE_SINCOS)

inline void sincos(double x, double* sinx, double* cosx) { ::sincos(x, sinx, cosx); }

#else

inline void sincos(double x, double* sinx, double* cosx)
{
    *sinx = ::sin(x);
    *cosx = ::cos(x);
}

#endif

// ----------------------------------------------------------------

#if defined(HAVE_SINCOSF)

inline void sincosf(float x, float* sinx, float* cosx) { ::sincosf(x, sinx, cosx); }

#elif defined(HAVE_SINF) && defined(HAVE_COSF)

inline void sincosf(float x, float* sinx, float* cosx)
{
    *sinx = ::sinf(x);
    *cosx = ::cosf(x);
}

#else

inline void sincosf(float x, float* sinx, float* cosx)
{
    *sinx = ::sin(x);
    *cosx = ::cos(x);
}

#endif

} // namespace gr

#endif /* INCLUDED_GR_SINCOS_H */
