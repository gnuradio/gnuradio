/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_basic_block.h>
#include <stdexcept>

using namespace pmt;

static long s_next_id = 0;
static long s_ncurrently_allocated = 0;

long 
gr_basic_block_ncurrently_allocated()
{
    return s_ncurrently_allocated;
}

gr_basic_block::gr_basic_block(const std::string &name,
                               gr_io_signature_sptr input_signature,
                               gr_io_signature_sptr output_signature) 
  : d_name(name),
    d_input_signature(input_signature),
    d_output_signature(output_signature),
    d_unique_id(s_next_id++),
    d_color(WHITE)
{
    s_ncurrently_allocated++;
}
  
gr_basic_block::~gr_basic_block()
{
    s_ncurrently_allocated--;
}

gr_basic_block_sptr 
gr_basic_block::to_basic_block()
{
    return shared_from_this();
}
