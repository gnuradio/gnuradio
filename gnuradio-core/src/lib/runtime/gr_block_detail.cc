/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <gr_block_detail.h>
#include <gr_buffer.h>

static long s_ncurrently_allocated = 0;

long
gr_block_detail_ncurrently_allocated ()
{
  return s_ncurrently_allocated;
}

gr_block_detail::gr_block_detail (unsigned int ninputs, unsigned int noutputs)
  : d_ninputs (ninputs), d_noutputs (noutputs),
    d_input (ninputs), d_output (noutputs),
    d_done (false)
{
  s_ncurrently_allocated++;
}

gr_block_detail::~gr_block_detail ()
{
  // should take care of itself
  s_ncurrently_allocated--;
}

void
gr_block_detail::set_input (unsigned int which, gr_buffer_reader_sptr reader)
{
  if (which >= d_ninputs)
    throw std::invalid_argument ("gr_block_detail::set_input");

  d_input[which] = reader;
}

void
gr_block_detail::set_output (unsigned int which, gr_buffer_sptr buffer)
{
  if (which >= d_noutputs)
    throw std::invalid_argument ("gr_block_detail::set_output");

  d_output[which] = buffer;
}

gr_block_detail_sptr
gr_make_block_detail (unsigned int ninputs, unsigned int noutputs)
{
  return gr_block_detail_sptr (new gr_block_detail (ninputs, noutputs));
}

void
gr_block_detail::set_done (bool done)
{
  d_done = done;
  for (unsigned int i = 0; i < d_noutputs; i++)
    d_output[i]->set_done (done);

  for (unsigned int i = 0; i < d_ninputs; i++)
    d_input[i]->set_done (done);
}

void 
gr_block_detail::consume (int which_input, int how_many_items)
{
  if (how_many_items > 0)
    input (which_input)->update_read_pointer (how_many_items);
}

void
gr_block_detail::consume_each (int how_many_items)
{
  if (how_many_items > 0)
    for (int i = 0; i < ninputs (); i++)
      d_input[i]->update_read_pointer (how_many_items);
}

void
gr_block_detail::produce_each (int how_many_items)
{
  if (how_many_items > 0)
    for (int i = 0; i < noutputs (); i++)
      d_output[i]->update_write_pointer (how_many_items);
}
