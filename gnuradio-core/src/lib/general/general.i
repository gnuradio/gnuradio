/* -*- c++ -*- */
/*
 * Copyright 2004-2012 Free Software Foundation, Inc.
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

%{

#include <gr_lfsr_32k_source_s.h>
#include <gr_check_lfsr_32k_s.h>
#include <gr_prefs.h>
#include <gr_constants.h>
#include <gr_test_types.h>
#include <gr_test.h>
#include <gr_feval.h>
#include <complex_vec_test.h>
#include <gr_endianness.h>
%}

%include "gr_lfsr_32k_source_s.i"
%include "gr_check_lfsr_32k_s.i"
%include "gr_prefs.i"
%include "gr_constants.i"
%include "gr_test_types.h"
%include "gr_test.i"
%include "gr_feval.i"
%include "complex_vec_test.i"
%include "gr_block_gateway.i"
%include "gr_endianness.h"
