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

typedef std::vector<int> gr_vector_int;
typedef std::vector<unsigned int> gr_vector_uint;
typedef std::vector<float> gr_vector_float;
typedef std::vector<double> gr_vector_double;
typedef std::vector<void*> gr_vector_void_star;
typedef std::vector<const void*> gr_vector_const_void_star;

#endif /* INCLUDED_GR_TYPES_H */
