/* -*- c++ -*- */
/*
 * Copyright 2005,2009,2012 Free Software Foundation, Inc.
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

#define COMEDI_API

%include "gnuradio.i"

//load generated python docstrings
%include "comedi_swig_doc.i"

%{
#include "gnuradio/comedi/sink_s.h"
#include "gnuradio/comedi/source_s.h"
%}

%include "gnuradio/comedi/sink_s.h"
%include "gnuradio/comedi/source_s.h"

GR_SWIG_BLOCK_MAGIC2(comedi, sink_s);
GR_SWIG_BLOCK_MAGIC2(comedi, source_s);
