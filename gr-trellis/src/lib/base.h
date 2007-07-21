/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_BASE_H
#define INCLUDED_TRELLIS_BASE_H

#include <vector>

/*!
 * \brief  change base
 */


bool dec2base(unsigned int num, int base, std::vector<int> &s);
bool dec2bases(unsigned int num, const std::vector<int> &bases, std::vector<int> &s);
unsigned int base2dec(const std::vector<int> &s, int base);
unsigned int bases2dec(const std::vector<int> &s, const std::vector<int> &bases);

#endif
