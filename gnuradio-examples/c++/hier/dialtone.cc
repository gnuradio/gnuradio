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

#include <dialtone.h>
#include <gr_io_signature.h>
#include <source.h>
#include <gr_kludge_copy.h>
#include <sink.h>

// Shared pointer constructor
dialtone_sptr make_dialtone()
{
    return dialtone_sptr(new dialtone());
}

// Hierarchical block constructor, with no inputs or outputs
dialtone::dialtone() : 
gr_hier_block2("dialtone",
	       gr_make_io_signature(0,0,0),
	       gr_make_io_signature(0,0,0))
{
    define_component("source", make_source());
    define_component("copy", gr_make_kludge_copy(sizeof(float)));
    define_component("sink", make_sink());

    connect("source", 0, "copy", 0);
    connect("source", 1, "copy", 1);
    connect("copy", 0, "sink",   0);
    connect("copy", 1, "sink",   1);
}
