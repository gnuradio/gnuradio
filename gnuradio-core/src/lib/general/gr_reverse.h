/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_REVERSE_H
#define INCLUDED_GR_REVERSE_H

#include <gr_core_api.h>
#include <vector>
#include <gr_complex.h>

// reverse the order of taps
std::vector<float> gr_reverse (const std::vector<float> &taps);
std::vector<gr_complex> gr_reverse (const std::vector<gr_complex> &taps);


#endif /* INCLUDED_GR_REVERSE_H */
