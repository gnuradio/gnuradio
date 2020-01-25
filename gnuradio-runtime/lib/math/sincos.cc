/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE // ask for GNU extensions if available
#endif

#include <gnuradio/sincos.h>
#include <math.h>

namespace gr {

#if defined(HAVE_SINCOS)

void sincos(double x, double* sinx, double* cosx) { ::sincos(x, sinx, cosx); }

#else

void sincos(double x, double* sinx, double* cosx)
{
    *sinx = ::sin(x);
    *cosx = ::cos(x);
}

#endif

// ----------------------------------------------------------------

#if defined(HAVE_SINCOSF)

void sincosf(float x, float* sinx, float* cosx) { ::sincosf(x, sinx, cosx); }

#elif defined(HAVE_SINF) && defined(HAVE_COSF)

void sincosf(float x, float* sinx, float* cosx)
{
    *sinx = ::sinf(x);
    *cosx = ::cosf(x);
}

#else

void sincosf(float x, float* sinx, float* cosx)
{
    *sinx = ::sin(x);
    *cosx = ::cos(x);
}

#endif

} /* namespace gr */
