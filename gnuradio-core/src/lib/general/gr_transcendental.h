/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_TRANSCENDENTAL_H
#define INCLUDED_GR_TRANSCENDENTAL_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <string>

/*!
 * \brief A block that performs various transcendental math operations.
 *
 * Possible function names can be found in the cmath library.
 * IO may be either complex or real, double or single precision.
 *
 * Possible type strings: float, double, complex_float, complex_double
 *
 * output[i] = trans_fcn(input[i])
 */
class GR_CORE_API gr_transcendental : virtual public gr_sync_block{
public:
    typedef boost::shared_ptr<gr_transcendental> sptr;
};

GR_CORE_API gr_transcendental::sptr gr_make_transcendental(
    const std::string &name,
    const std::string &type = "float"
);

#endif /* INCLUDED_GR_TRANSCENDENTAL_H */
