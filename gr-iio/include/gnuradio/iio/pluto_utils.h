/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_PLUTO_UTILS_H
#define INCLUDED_IIO_PLUTO_UTILS_H

#include <gnuradio/iio/api.h>
#include <string>

namespace gr {
namespace iio {
std::string IIO_API get_pluto_uri();
}
} // namespace gr
#endif