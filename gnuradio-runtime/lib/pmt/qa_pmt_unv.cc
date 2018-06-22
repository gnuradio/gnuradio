/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
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

// We need a non-generated C++ file for CMake, all the actual test cases are
// generated from the template and written to qa_pmt_unv.h

#include <pmt/api.h> //reason: suppress warnings
#include <gnuradio/messages/msg_passing.h>
#include <boost/test/unit_test.hpp>
#include <boost/format.hpp>
#include <cstdio>
#include <cstring>
#include <sstream>

#include "qa_pmt_unv.h"
