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
#include <gr_random_annotator.h>
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

  //CPPUNIT_ASSERT_THROW(src->nitems_read(0), std::runtime_error);
  //CPPUNIT_ASSERT_THROW(src->nitems_written(0), std::runtime_error);
  CPPUNIT_ASSERT_EQUAL(src->nitems_read(0), (uint64_t)0);
  CPPUNIT_ASSERT_EQUAL(src->nitems_written(0), (uint64_t)0);

  tb->run();

  CPPUNIT_ASSERT_THROW(src->nitems_read(0), std::invalid_argument);
  CPPUNIT_ASSERT(src->nitems_written(0) >= N);
  CPPUNIT_ASSERT_EQUAL(snk->nitems_read(0), (uint64_t)1000);
  CPPUNIT_ASSERT_THROW(snk->nitems_written(0), std::invalid_argument);
}


void
qa_block_tags::t1 ()
{
  printf("\nqa_block_tags::t1\n");

  int N = 40000;
  gr_top_block_sptr tb = gr_make_top_block("top");
  gr_block_sptr src (gr_make_null_source(sizeof(int)));
  gr_block_sptr head (gr_make_head(sizeof(int), N));
  gr_block_sptr ann0 (gr_make_random_annotator(sizeof(int)));
  gr_block_sptr ann1 (gr_make_random_annotator(sizeof(int)));
  gr_block_sptr ann2 (gr_make_random_annotator(sizeof(int)));
  gr_block_sptr snk (gr_make_null_sink(sizeof(int)));
  
  tb->connect(src, 0, head, 0);
  tb->connect(head, 0, ann0, 0);
  tb->connect(ann0, 0, ann1, 0);
  tb->connect(ann1, 0, ann2, 0);
  tb->connect(ann2, 0, snk, 0);
  tb->run();

}

