/* -*- c++ -*- */
/*
 * Copyright 2003, 2008 Free Software Foundation, Inc.
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

#ifndef _RANDOM_H_
#define _RANDOM_H_

// While rand(3) specifies RAND_MAX, random(3) says that the output
// ranges from 0 to 2^31-1 but does not specify a macro to denote
// this.  We define RANDOM_MAX for cleanliness.  We must omit the
// definition for systems that have made the same choice.  (Note that
// random(3) is from 4.2BSD, and not specified by POSIX.)

#ifndef RANDOM_MAX
static const int RANDOM_MAX = 2147483647; // 2^31-1
#endif /* RANDOM_MAX */

#include <stdlib.h>

#endif // _RANDOM_H_
