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

#include <output.h>
#include <gr_io_signature.h>
#include <audio_alsa_sink.h>

// Shared pointer constructor
output_sptr make_output()
{
    return output_sptr(new output());
}

output::output() : 
gr_hier_block2("output",
	       gr_make_io_signature(2,2,sizeof(float)),
	       gr_make_io_signature(0,0,0))
{
    define_component("audio", audio_alsa_make_sink(48000));
    
    connect("self", 0, "audio", 0);
    connect("self", 1, "audio", 1);
}
