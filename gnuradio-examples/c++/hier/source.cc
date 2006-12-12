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

#include <source.h>
#include <gr_io_signature.h>
#include <siggen.h>

// Shared pointer constructor
source_sptr make_source()
{
    return source_sptr(new source());
}

source::source() : 
gr_hier_block2("source",
	       gr_make_io_signature(0,0,0),
	       gr_make_io_signature(2,2,sizeof(float)))
{
    define_component("siggen", make_siggen());

    connect("siggen", 0, "self", 0);
    connect("siggen", 1, "self", 1);
}
