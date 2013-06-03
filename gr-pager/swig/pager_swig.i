/*
 * Copyright 2005,2006,2009,2012 Free Software Foundation, Inc.
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

#define PAGER_API

%include "gnuradio.i"

//load generated python docstrings
%include "pager_swig_doc.i"

%{
#include "gnuradio/pager/flex_frame.h"
#include "gnuradio/pager/slicer_fb.h"
#include "gnuradio/pager/flex_sync.h"
#include "gnuradio/pager/flex_deinterleave.h"
#include "gnuradio/pager/flex_parse.h"
%}

%include "gnuradio/pager/flex_frame.h"
%include "gnuradio/pager/slicer_fb.h"
%include "gnuradio/pager/flex_sync.h"
%include "gnuradio/pager/flex_deinterleave.h"
%include "gnuradio/pager/flex_parse.h"

GR_SWIG_BLOCK_MAGIC2(pager, flex_frame);
GR_SWIG_BLOCK_MAGIC2(pager, slicer_fb);
GR_SWIG_BLOCK_MAGIC2(pager, flex_sync);
GR_SWIG_BLOCK_MAGIC2(pager, flex_deinterleave);
GR_SWIG_BLOCK_MAGIC2(pager, flex_parse);
