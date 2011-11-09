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

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gr_basic_add.h>
#include <gr_basic_add_const.h>
#include <gr_basic_divide.h>
#include <gr_basic_multiply.h>
#include <gr_basic_multiply_const.h>
#include <gr_basic_subtract.h>
%}

////////////////////////////////////////////////////////////////////////
// import types
////////////////////////////////////////////////////////////////////////
%include <gr_basic_op_types.h>

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_SWIG_BLOCK_MAGIC(basic,add)
%include <gr_basic_add.h>

GR_SWIG_BLOCK_MAGIC(basic,add_const)
%include <gr_basic_add_const.h>

GR_SWIG_BLOCK_MAGIC(basic,divide)
%include <gr_basic_divide.h>

GR_SWIG_BLOCK_MAGIC(basic,multiply)
%include <gr_basic_multiply.h>

GR_SWIG_BLOCK_MAGIC(basic,multiply_const)
%include <gr_basic_multiply_const.h>

GR_SWIG_BLOCK_MAGIC(basic,subtract)
%include <gr_basic_subtract.h>
