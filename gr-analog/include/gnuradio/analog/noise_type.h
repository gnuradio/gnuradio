/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_NOISE_TYPE_H
#define INCLUDED_ANALOG_NOISE_TYPE_H

namespace gr {
namespace analog {

enum noise_type_t { GR_UNIFORM = 200, GR_GAUSSIAN, GR_LAPLACIAN, GR_IMPULSE };

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_NOISE_TYPE_H */
