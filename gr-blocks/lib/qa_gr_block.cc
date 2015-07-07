/* -*- c++ -*- */
/*
 * Copyright 2004.2013 Free Software Foundation, Inc.
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

#include <qa_gr_block.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>

// ----------------------------------------------------------------


void
qa_gr_block::t0 ()
{
  // test creation of sources
  gr::block_sptr src1(gr::blocks::null_source::make(sizeof (int)));
  CPPUNIT_ASSERT_EQUAL(std::string("null_source"), src1->name ());
  CPPUNIT_ASSERT_EQUAL(0, src1->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL(1, src1->output_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL(-1, src1->output_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL((int) sizeof(int),
                       src1->output_signature()->sizeof_stream_item (0));

  gr::block_sptr src2(gr::blocks::null_source::make(sizeof(short)));
  CPPUNIT_ASSERT_EQUAL(std::string ("null_source"), src2->name ());
  CPPUNIT_ASSERT_EQUAL(0, src2->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL(1, src2->output_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL(-1, src2->output_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL((int)sizeof (short),
                       src2->output_signature()->sizeof_stream_item (0));
}


void
qa_gr_block::t1 ()
{
  // test creation of sinks
  gr::block_sptr dst1 (gr::blocks::null_sink::make (sizeof (int)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_sink"), dst1->name ());
  CPPUNIT_ASSERT_EQUAL (1, dst1->input_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (-1, dst1->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof (int),
            dst1->input_signature()->sizeof_stream_item (0));

  CPPUNIT_ASSERT_EQUAL (0, dst1->output_signature()->max_streams ());

  gr::block_sptr dst2 (gr::blocks::null_sink::make (sizeof (short)));
  CPPUNIT_ASSERT_EQUAL (std::string ("null_sink"), dst2->name ());
  CPPUNIT_ASSERT_EQUAL (1, dst2->input_signature()->min_streams ());
  CPPUNIT_ASSERT_EQUAL (-1, dst2->input_signature()->max_streams ());
  CPPUNIT_ASSERT_EQUAL ((int) sizeof (short),
            dst2->input_signature()->sizeof_stream_item (0));
  CPPUNIT_ASSERT_EQUAL (0, dst2->output_signature()->max_streams ());
}

#include <gnuradio/top_block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/blocks/nop.h>

void
qa_gr_block::t2 ()
{
  gr::block_sptr src1(gr::blocks::null_source::make(sizeof (int)));
  gr::block_sptr nop(gr::blocks::nop::make(sizeof (int)));
  gr::block_sptr dst1 (gr::blocks::null_sink::make (sizeof (int)));

  gr::top_block_sptr tb(gr::make_top_block("t2"));
  tb->connect(src1, 0, nop, 0);
  tb->connect(nop, 0, dst1, 0);
  tb->start();

  const char *obuf = nop->detail()->output(0)->base();
  int obsize = nop->detail()->output(0)->bufsize();
  const char *ibuf = nop->detail()->input(0)->buffer()->base();
  int ibsize = nop->detail()->input(0)->buffer()->bufsize();

  CPPUNIT_ASSERT(obuf != NULL);
  CPPUNIT_ASSERT(ibuf != NULL);
  CPPUNIT_ASSERT(obsize > 0);
  CPPUNIT_ASSERT(ibsize > 0);

  tb->stop();
  tb->wait();
}

void
qa_gr_block::t3 ()
{
}
