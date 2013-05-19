/* -*- c++ -*- */
/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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
// standard includes
////////////////////////////////////////////////////////////////////////
%include <gnuradio.i>
%include <tags.i>
%import <feval.i>

%template(void_start_vector_t) std::vector<void *>;

////////////////////////////////////////////////////////////////////////
// block headers
////////////////////////////////////////////////////////////////////////
%{
#include <gnuradio/block_gateway.h>
%}

////////////////////////////////////////////////////////////////////////
// block magic
////////////////////////////////////////////////////////////////////////
%include <gnuradio/block_gateway.h>

%template(block_gateway_sptr) boost::shared_ptr<gr::block_gateway>;
%pythoncode %{
block_gateway_sptr.__repr__ = lambda self: "<block_gateway>"
block_gateway = block_gateway.make;
%}

