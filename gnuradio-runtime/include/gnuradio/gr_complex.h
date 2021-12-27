/* -*- c++ -*- */
/*
 * Copyright 2004,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_COMPLEX_H
#define INCLUDED_GR_COMPLEX_H

#include <complex>
using gr_complex = std::complex<float>;
using gr_complexd = std::complex<double>;

inline bool is_complex(gr_complex x)
{
    (void)x;
    return true;
}
inline bool is_complex(gr_complexd x)
{
    (void)x;
    return true;
}
inline bool is_complex(float x)
{
    (void)x;
    return false;
}
inline bool is_complex(double x)
{
    (void)x;
    return false;
}
inline bool is_complex(int x)
{
    (void)x;
    return false;
}
inline bool is_complex(char x)
{
    (void)x;
    return false;
}
inline bool is_complex(short x)
{
    (void)x;
    return false;
}

#endif /* INCLUDED_GR_COMPLEX_H */
