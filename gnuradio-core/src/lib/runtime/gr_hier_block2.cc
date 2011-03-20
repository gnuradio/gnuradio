/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
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

#include <gr_hier_block2.h>
#include <gr_io_signature.h>
#include <gr_hier_block2_detail.h>
#include <iostream>

#define GR_HIER_BLOCK2_DEBUG 0


gr_hier_block2_sptr
gr_make_hier_block2(const std::string &name, 
                    gr_io_signature_sptr input_signature,
                    gr_io_signature_sptr output_signature)
{
  return gnuradio::get_initial_sptr(new gr_hier_block2(name, input_signature, output_signature));
}

gr_hier_block2::gr_hier_block2(const std::string &name,
                               gr_io_signature_sptr input_signature,
                               gr_io_signature_sptr output_signature)
  : gr_basic_block(name, input_signature, output_signature),
    d_detail(new gr_hier_block2_detail(this))
{
  // This bit of magic ensures that self() works in the constructors of derived classes.
  gnuradio::detail::sptr_magic::create_and_stash_initial_sptr(this);
}

gr_hier_block2::~gr_hier_block2()
{
  delete d_detail;
}

gr_hier_block2::opaque_self
gr_hier_block2::self()
{
  return shared_from_this();
}

gr_hier_block2_sptr
gr_hier_block2::to_hier_block2()
{
  return cast_to_hier_block2_sptr(shared_from_this());
}

void 
gr_hier_block2::connect(gr_basic_block_sptr block)
{
  d_detail->connect(block);
}

void 
gr_hier_block2::connect(gr_basic_block_sptr src, int src_port, 
                        gr_basic_block_sptr dst, int dst_port)
{
  d_detail->connect(src, src_port, dst, dst_port);
}

void 
gr_hier_block2::disconnect(gr_basic_block_sptr block)
{
  d_detail->disconnect(block);
}

void 
gr_hier_block2::disconnect(gr_basic_block_sptr src, int src_port, 
                           gr_basic_block_sptr dst, int dst_port)
{
  d_detail->disconnect(src, src_port, dst, dst_port);
}

void
gr_hier_block2::disconnect_all()
{
  d_detail->disconnect_all();
}

void
gr_hier_block2::lock()
{
  d_detail->lock();
}

void
gr_hier_block2::unlock()
{
  d_detail->unlock();
}

gr_flat_flowgraph_sptr
gr_hier_block2::flatten() const
{
  gr_flat_flowgraph_sptr new_ffg = gr_make_flat_flowgraph();
  d_detail->flatten_aux(new_ffg);
  return new_ffg;
}
