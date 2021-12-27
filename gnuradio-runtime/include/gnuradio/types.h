/* -*- c++ -*- */
/*
 * Copyright 2004,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TYPES_H
#define INCLUDED_GR_TYPES_H

#include <gnuradio/api.h>
#include <cstddef> // size_t
#include <memory>
#include <vector>

#include <gnuradio/gr_complex.h>

#include <cstdint>

using gr_vector_int = std::vector<int>;
using gr_vector_uint = std::vector<unsigned int>;
using gr_vector_float = std::vector<float>;
using gr_vector_double = std::vector<double>;
using gr_vector_void_star = std::vector<void*>;
using gr_vector_const_void_star = std::vector<const void*>;

#endif /* INCLUDED_GR_TYPES_H */
