/* -*- c++ -*- */
/*
 * Copyright 2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MISC_H
#define INCLUDED_GR_MISC_H

#include <gnuradio/api.h>
#include <gnuradio/types.h>

namespace gr {

GR_RUNTIME_API unsigned int rounduppow2(unsigned int n);

// FIXME should be template
GR_RUNTIME_API void zero_vector(std::vector<float>& v);
GR_RUNTIME_API void zero_vector(std::vector<double>& v);
GR_RUNTIME_API void zero_vector(std::vector<int>& v);
GR_RUNTIME_API void zero_vector(std::vector<gr_complex>& v);

} /* namespace gr */

#endif /* INCLUDED_GR_MISC_H */
