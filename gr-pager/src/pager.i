/*
 * Copyright 2005,2006 Free Software Foundation, Inc.
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

%feature("autodoc","1");
%include "exception.i"
%import "gnuradio.i"

%{
#include "gnuradio_swig_bug_workaround.h"	// mandatory bug fix
#include "pager_flex_frame.h"
#include "pager_slicer_fb.h"
#include "pager_flex_sync.h"
#include "pager_flex_deinterleave.h"
#include "pager_flex_parse.h"
#include <stdexcept>
%}

%include "pager_flex_frame.i"

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(pager,slicer_fb);

pager_slicer_fb_sptr pager_make_slicer_fb(float alpha);

class pager_slicer_fb : public gr_sync_block
{
private:
    pager_slicer_fb(float alpha);

public:
    float dc_offset() const { return d_avg; }
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(pager,flex_sync);

pager_flex_sync_sptr pager_make_flex_sync();

class pager_flex_sync : public gr_block
{
private:
    pager_flex_sync();

public:
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(pager,flex_deinterleave);

pager_flex_deinterleave_sptr pager_make_flex_deinterleave();

class pager_flex_deinterleave : public gr_sync_decimator
{
private:
    pager_flex_deinterleave();

public:
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(pager,flex_parse);

pager_flex_parse_sptr pager_make_flex_parse(gr_msg_queue_sptr queue, float freq);

class pager_flex_parse : public gr_block
{
private:
    pager_flex_parse(gr_msg_queue_sptr queue, float freq);

public:
};
