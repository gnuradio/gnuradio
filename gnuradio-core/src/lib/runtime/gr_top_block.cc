/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <unistd.h>
#include <gr_top_block.h>
#include <gr_top_block_impl.h>
#include <gr_io_signature.h>
#include <iostream>

gr_top_block_sptr 
gr_make_top_block(const std::string &name)
{
  return gnuradio::get_initial_sptr(new gr_top_block(name));
}

gr_top_block::gr_top_block(const std::string &name)
  : gr_hier_block2(name, 
		   gr_make_io_signature(0,0,0), 
		   gr_make_io_signature(0,0,0))
  
{
  d_impl = new gr_top_block_impl(this);
}
  
gr_top_block::~gr_top_block()
{
  stop();
  wait();
    
  delete d_impl;
}

void 
gr_top_block::start(int max_noutput_items)
{
  d_impl->start(max_noutput_items);
}

void 
gr_top_block::stop()
{
  d_impl->stop();
}

void 
gr_top_block::wait()
{
  d_impl->wait();
}

void 
gr_top_block::run(int max_noutput_items)
{
  start(max_noutput_items);
  wait();
}

void
gr_top_block::lock()
{
  d_impl->lock();
}

void
gr_top_block::unlock()
{
  d_impl->unlock();
}

void
gr_top_block::dump()
{
  d_impl->dump();
}

int
gr_top_block::max_noutput_items()
{
  return d_impl->max_noutput_items();
}

void
gr_top_block::set_max_noutput_items(int nmax)
{
  d_impl->set_max_noutput_items(nmax);
}

gr_top_block_sptr
gr_top_block::to_top_block()
{
  return cast_to_top_block_sptr(shared_from_this());
}
