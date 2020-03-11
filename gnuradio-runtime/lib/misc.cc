/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "misc.h"

namespace gr {

unsigned int rounduppow2(unsigned int n)
{
    int i;
    for (i = 0; ((n - 1) >> i) != 0; i++)
        ;
    return 1 << i;
}

// ----------------------------------------------------------------

void zero_vector(std::vector<float>& v)
{
    for (unsigned int i = 0; i < v.size(); i++)
        v[i] = 0;
}

void zero_vector(std::vector<double>& v)
{
    for (unsigned int i = 0; i < v.size(); i++)
        v[i] = 0;
}

void zero_vector(std::vector<int>& v)
{
    for (unsigned int i = 0; i < v.size(); i++)
        v[i] = 0;
}

void zero_vector(std::vector<gr_complex>& v)
{
    for (unsigned int i = 0; i < v.size(); i++)
        v[i] = 0;
}

} /* namespace gr */
