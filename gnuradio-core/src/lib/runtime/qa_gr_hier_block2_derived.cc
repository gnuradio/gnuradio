/* -*- c++ -*- */
/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <qa_gr_hier_block2_derived.h>
#include <gr_top_block.h>
#include <gr_io_signature.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>
#include <gr_head.h>
#include <gr_kludge_copy.h>

// Declare a test C++ hierarchical block

class gr_derived_block;
typedef boost::shared_ptr<gr_derived_block> gr_derived_block_sptr;
gr_derived_block_sptr gr_make_derived_block();

class gr_derived_block : public gr_hier_block2
{
private:
  friend gr_derived_block_sptr gr_make_derived_block();
  gr_derived_block();

public:
  ~gr_derived_block();
};


gr_derived_block_sptr
gr_make_derived_block()
{
  return gnuradio::get_initial_sptr(new gr_derived_block());
}

gr_derived_block::gr_derived_block()
  : gr_hier_block2("gr_derived_block",
		   gr_make_io_signature(1, 1, sizeof(int)), // Input signature
		   gr_make_io_signature(1, 1, sizeof(int))) // Output signature
{
  gr_block_sptr copy(gr_make_kludge_copy(sizeof(int)));

  connect(self(), 0, copy, 0);
  connect(copy, 0, self(), 0);
}

gr_derived_block::~gr_derived_block()
{
}

void qa_gr_hier_block2_derived::test_1()
{
  gr_top_block_sptr     tb(gr_make_top_block("test"));

  gr_block_sptr         src(gr_make_null_source(sizeof(int)));
  gr_block_sptr         head(gr_make_head(sizeof(int), 1000));
  gr_derived_block_sptr blk(gr_make_derived_block());
  gr_block_sptr         dst(gr_make_null_sink(sizeof(int)));
  
  tb->connect(src,  0, head, 0);
  tb->connect(head, 0, blk,  0);
  tb->connect(blk,  0, dst,  0);

  tb->run();
}
