/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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
#include <qa_block_tags.h>
#include <gr_block.h>
#include <gr_top_block.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>
#include <gr_head.h>
#include <gruel/pmt.h>


// ----------------------------------------------------------------


void
qa_block_tags::t0 ()
{
  printf("\nqa_block_tags::t0\n");

  unsigned int N = 1000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_block_sptr snk (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, snk, 0);

  //CPPUNIT_ASSERT_THROW(src->n_items_read(0), std::runtime_error);
  //CPPUNIT_ASSERT_THROW(src->n_items_written(0), std::runtime_error);
  CPPUNIT_ASSERT_EQUAL(src->n_items_read(0), (gr_uint64)0);
  CPPUNIT_ASSERT_EQUAL(src->n_items_written(0), (gr_uint64)0);

  tb->run();

  CPPUNIT_ASSERT_THROW(src->n_items_read(0), std::invalid_argument);
  CPPUNIT_ASSERT(src->n_items_written(0) >= N);
  CPPUNIT_ASSERT_EQUAL(snk->n_items_read(0), (gr_uint64)1000);
  CPPUNIT_ASSERT_THROW(snk->n_items_written(0), std::invalid_argument);
}


void
qa_block_tags::t1 ()
{
  printf("\nqa_block_tags::t1\n");

  int N = 1000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_block_sptr snk (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, snk, 0);
  tb->run();

  gr_uint64 W = src->n_items_written(0);
  src->add_item_tag(0, N,
		    pmt::pmt_string_to_symbol("test1"),
		    pmt::pmt_from_double(1.234));
  
  // Make sure we can't get duplicates
  src->add_item_tag(0, N,
		    pmt::pmt_string_to_symbol("test1"),
		    pmt::pmt_from_double(1.234));

  // Add new tag at another position
  src->add_item_tag(0, W,
		    pmt::pmt_string_to_symbol("test2"),
		    pmt::pmt_from_double(2.345));

  // Test how many tags we get for different ranges
  // should be 1, 0, 0, and 2
  std::list<pmt::pmt_t> tags0, tags1, tags2, tags3;
  tags0 = src->get_tags_in_range(0, N-10, N+10);
  tags1 = src->get_tags_in_range(0, N-10, N- 1);
  tags2 = src->get_tags_in_range(0, N+ 1, N+10);
  tags3 = src->get_tags_in_range(0, 0, W);
  
  CPPUNIT_ASSERT(tags0.size() == 1);
  CPPUNIT_ASSERT(tags1.size() == 0);
  CPPUNIT_ASSERT(tags2.size() == 0);
  CPPUNIT_ASSERT(tags3.size() == 2);

  // Check types and values are good
  pmt::pmt_t tuple = tags0.front();
  pmt::pmt_t key = pmt::pmt_tuple_ref(tuple, 2);
  double value = pmt::pmt_to_double(pmt::pmt_tuple_ref(tuple, 3));
  
  CPPUNIT_ASSERT(pmt::pmt_is_tuple(tuple));
  CPPUNIT_ASSERT(pmt::pmt_eqv(key, pmt::pmt_string_to_symbol("test1")));
  CPPUNIT_ASSERT_EQUAL(value, 1.234);
}

