/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_GR_EXPJ_H
#define INCLUDED_GR_EXPJ_H

#include <gnuradio/api.h>
#include <gnuradio/sincos.h>
#include <gnuradio/types.h>

static inline gr_complex gr_expj(float phase)
{
    float t_imag, t_real;
    gr::sincosf(phase, &t_imag, &t_real);
    return gr_complex(t_real, t_imag);
}


#endif /* INCLUDED_GR_EXPJ_H */
