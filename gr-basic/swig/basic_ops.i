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

%include <gr_basic_add.h>
%include <gr_basic_add_const.h>
%include <gr_basic_divide.h>
%include <gr_basic_multiply.h>
%include <gr_basic_multiply_const.h>
%include <gr_basic_subtract.h>

////////////////////////////////////////////////////////////////////////
// import types
////////////////////////////////////////////////////////////////////////
%include <gr_basic_op_types.h>

////////////////////////////////////////////////////////////////////////
// template foo
////////////////////////////////////////////////////////////////////////
%template(set_value) gr_basic_add_const::set_value<std::complex<double> >;
%template(set_value) gr_basic_multiply_const::set_value<std::complex<double> >;

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
GR_SWIG_BLOCK_MAGIC2(gr_basic,add)
GR_SWIG_BLOCK_MAGIC2(gr_basic,add_const)
GR_SWIG_BLOCK_MAGIC2(gr_basic,divide)
GR_SWIG_BLOCK_MAGIC2(gr_basic,multiply)
GR_SWIG_BLOCK_MAGIC2(gr_basic,multiply_const)
GR_SWIG_BLOCK_MAGIC2(gr_basic,subtract)
