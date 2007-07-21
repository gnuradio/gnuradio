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
#ifndef _FPLL_BTLOOP_COUPLING_H_
#define _FPLL_BTLOOP_COUPLING_H_

/*!
 * Magic coupling constant between GrAtscFPLL and GrAtscBitTimingLoop.
 * Trust me, you don't want to mess with this.
 *
 * The agc block buried in the FPLL indirectly sets the level of the input
 * to the bit timing loop.  The bit timing loop's convergence properties
 * depend on the the mean amplitude of it's input.  This constant ties
 * them together such that you can tweak the input level of the bit timing loop
 * (somewhat) without screwing everything.
 */

static const float FPLL_BTLOOP_COUPLING_CONST = 3.125;

#endif /* _FPLL_BTLOOP_COUPLING_H_ */
