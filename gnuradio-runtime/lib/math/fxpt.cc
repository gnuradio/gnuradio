/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fxpt.h>

namespace gr {

const float fxpt::s_sine_table[1 << NBITS][2] = {
#include "generated/sine_table.h"
};

const float fxpt::PI = 3.14159265358979323846;
const float fxpt::TAU = 2.0 * 3.14159265358979323846;
const float fxpt::TWO_TO_THE_31 = 2147483648.0;

} /* namespace gr */
