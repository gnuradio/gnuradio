/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"	// the common stuff
%import <stdint.i>

%{
#include "vrt_quadradio_source_32fc.h"
#include <vrt/quadradio.h>
//#include "vrt_quadradio_source_16sc.h"
//#include "vrt_sink_32fc.h"
//#include "vrt_sink_16sc.h"
%}

%template(uint32_t_vector) std::vector<uint32_t>;

// ----------------------------------------------------------------

class vrt_source_base : public gr_sync_block
{
protected:
  vrt_source_base() throw (std::runtime_error);

public:
  ~vrt_source_base();

};

class vrt_source_32fc : public vrt_source_base
{
protected:
  vrt_source_32fc() throw (std::runtime_error);

public:
  ~vrt_source_32fc();
  void reset();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(vrt,quadradio_source_32fc)

%include "vrt_quadradio_source_32fc.h"

%include <vrt/quadradio.h>
