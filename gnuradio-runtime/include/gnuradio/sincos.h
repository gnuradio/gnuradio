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

namespace gr {

// compute sine and cosine at the same time
GR_RUNTIME_API void sincos(double x, double* sin, double* cos);
GR_RUNTIME_API void sincosf(float x, float* sin, float* cos);
} // namespace gr

#endif /* INCLUDED_GR_SINCOS_H */
