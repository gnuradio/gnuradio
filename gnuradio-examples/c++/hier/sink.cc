/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <sink.h>
#include <gr_io_signature.h>
#include <gr_kludge_copy.h>
#include <output.h>

// Shared pointer constructor
sink_sptr make_sink()
{
    return sink_sptr(new sink());
}

sink::sink() : 
gr_hier_block2("sink",
	       gr_make_io_signature(2,2,sizeof(float)),
	       gr_make_io_signature(0,0,0))
{
    define_component("copy", gr_make_kludge_copy(sizeof(float)));
    define_component("output", make_output());

    connect("self", 0, "copy",   0);
    connect("self", 1, "copy",   1);
    connect("copy",  0, "output", 0);
    connect("copy",  1, "output", 1);    
}
