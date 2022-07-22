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

#include <gnuradio/kernel/math/fxpt.h>

namespace gr {

const float kernel::math::fxpt::s_sine_table[1 << NBITS][2] = {
#include "sine_table.h"
};

const float kernel::math::fxpt::PI = 3.14159265358979323846;
const float kernel::math::fxpt::TAU = 2.0 * 3.14159265358979323846;
const float kernel::math::fxpt::TWO_TO_THE_31 = 2147483648.0;

} /* namespace gr */
