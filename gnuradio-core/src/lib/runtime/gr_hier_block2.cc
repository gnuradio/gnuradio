/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_hier_block2.h>
#include <gr_io_signature.h>
#include <gr_hier_block2_detail.h>
#include <iostream>

#define GR_HIER_BLOCK2_DEBUG 1

gr_hier_block2_sptr gr_make_hier_block2(const std::string &name, 
                                        gr_io_signature_sptr input_signature,
                                        gr_io_signature_sptr output_signature)
{
    return gr_hier_block2_sptr(new gr_hier_block2(name, input_signature, output_signature));
}

gr_hier_block2::gr_hier_block2(const std::string &name,
                               gr_io_signature_sptr input_signature,
                               gr_io_signature_sptr output_signature)
  : gr_basic_block(name, input_signature, output_signature),
    d_detail(new gr_hier_block2_detail(this))
{
}

gr_hier_block2::~gr_hier_block2()
{
    delete d_detail;
}

void 
gr_hier_block2::connect(gr_basic_block_sptr src, int src_port, 
                        gr_basic_block_sptr dst, int dst_port)
{
    d_detail->connect(src, src_port, dst, dst_port);
}

void 
gr_hier_block2::disconnect(gr_basic_block_sptr src, int src_port, 
                           gr_basic_block_sptr dst, int dst_port)
{
    d_detail->disconnect(src, src_port, dst, dst_port);
}

void
gr_hier_block2::set_runtime(gr_runtime *runtime)
{
    if (GR_HIER_BLOCK2_DEBUG)
      std::cout << "Setting runtime on " << this << " to " << runtime << std::endl;
    d_detail->set_runtime(runtime);
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
