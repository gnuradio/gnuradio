/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#define ANALOG_API

%include "gnuradio.i"

//load generated python docstrings
%include "analog_swig_doc.i"

%{
#include "analog/cpm.h"
#include "analog/ctcss_squelch_ff.h"
#include "analog/squelch_base_ff.h"
%}

%include "analog/cpm.h"
%include "analog/ctcss_squelch_ff.h"
%include "analog/squelch_base_ff.h"

GR_SWIG_BLOCK_MAGIC2(analog, ctcss_squelch_ff);
