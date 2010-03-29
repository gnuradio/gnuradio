/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

%include "gnuradio.i"

%{
#include <uhd_simple_source.h>
#include <uhd_simple_sink.h>
%}

%include <uhd/types/ranges.hpp>
%include <uhd/types/tune_result.hpp>

GR_SWIG_BLOCK_MAGIC(uhd,simple_source)
%include <uhd_simple_source.h>

GR_SWIG_BLOCK_MAGIC(uhd,simple_sink)
%include <uhd_simple_sink.h>
