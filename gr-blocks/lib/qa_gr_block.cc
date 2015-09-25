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
#include <gnuradio/top_block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/blocks/nop.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/null_source.h>

namespace gr {
  namespace qa {
    class test_custom_buffers_source : virtual public sync_block {
      public:
      typedef boost::shared_ptr<test_custom_buffers_source> sptr;
      static sptr make(size_t sizeof_stream_item);
    };

    class test_custom_buffers_source_impl : public test_custom_buffers_source {
      public:
      test_custom_buffers_source_impl(size_t sizeof_stream_item)
              : sync_block("test_custom_buffers_source", io_signature::make(0, 0, 0),
                           io_signature::make(1, 1, sizeof_stream_item, gr::io_signature::MEM_BLOCK_ALLOC))
      {}

      ~test_custom_buffers_source_impl()
      {}

      int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
      {
        void *optr;
        for (size_t n = 0; n < input_items.size(); n++) {
          optr = (void *) output_items[n];
          memset(optr, 0, noutput_items * output_signature()->sizeof_stream_item(n));
        }
        return noutput_items;
      }

      virtual buffer_sptr allocate_output_buffer(int port);
    };

    test_custom_buffers_source::sptr test_custom_buffers_source::make(size_t sizeof_stream_item)
    {
      return gnuradio::get_initial_sptr(new test_custom_buffers_source_impl(sizeof_stream_item));
    }

#define GR_FIXED_BUFFER_SIZE (32*(1L<<10))
    static const unsigned int s_fixed_buffer_size = GR_FIXED_BUFFER_SIZE;

    buffer_sptr test_custom_buffers_source_impl::allocate_output_buffer(int port)
    {
      // make output buffer
      int item_size = this->output_signature()->sizeof_stream_item(port);
      int nitems = s_fixed_buffer_size * 2 / item_size;
      // Here be dragons...
      block_sptr block_ptr = cast_to_block_sptr(shared_from_this()); // shared_from_this() to make sure we don't delete ourselves
      buffer_sptr buffer = make_buffer(nitems, item_size, block_ptr);
      buffer->allocate_buffer();
      return buffer;
    }
    class test_custom_buffers_sink : virtual public sync_block {
      public:
      typedef boost::shared_ptr<test_custom_buffers_sink> sptr;
      static sptr make(size_t sizeof_stream_item);
    };

    class test_custom_buffers_sink_impl : public test_custom_buffers_sink {
      public:
      test_custom_buffers_sink_impl(size_t sizeof_stream_item)
              : sync_block("test_custom_buffers_sink",
                           io_signature::make(1, 1, sizeof_stream_item, gr::io_signature::MEM_BLOCK_ALLOC),
                           io_signature::make(0, 0, 0))
      {}

      ~test_custom_buffers_sink_impl()
      {}

      int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items)
      {
        void *optr;
        for (size_t n = 0; n < input_items.size(); n++) {
          optr = (void *) output_items[n];
          memset(optr, 0, noutput_items * output_signature()->sizeof_stream_item(n));
        }
        return noutput_items;
      }

      virtual buffer_sptr allocate_input_buffer(int port);
    };

    test_custom_buffers_sink::sptr test_custom_buffers_sink::make(size_t sizeof_stream_item)
    {
      return gnuradio::get_initial_sptr(new test_custom_buffers_sink_impl(sizeof_stream_item));
    }

    buffer_sptr test_custom_buffers_sink_impl::allocate_input_buffer(int port)
    {
      // make output buffer
      int item_size = this->input_signature()->sizeof_stream_item(port);
      int nitems = s_fixed_buffer_size * 2 / item_size;
      // Here be dragons...
      block_sptr block_ptr = cast_to_block_sptr(shared_from_this()); // shared_from_this() to make sure we don't delete ourselves
      buffer_sptr my_buffer = make_buffer(nitems, item_size, block_ptr);
      my_buffer->allocate_buffer();
      return my_buffer;
    }
  }
}


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
qa_gr_block::t3_custom_allocator_output()
{
  gr::block_sptr src1 (gr::qa::test_custom_buffers_source::make (sizeof (int)));
  gr::block_sptr dst1 (gr::blocks::null_sink::make (sizeof (int)));

  gr::top_block_sptr tb (gr::make_top_block ("t3"));
  tb->connect (src1, 0, dst1, 0);
  tb->start();
  const char *obuf = src1->detail()->output(0)->base();
  int obsize = src1->detail()->output(0)->bufsize();
  const char *ibuf = dst1->detail()->input(0)->buffer()->base();
  int ibsize = dst1->detail()->input(0)->buffer()->bufsize();
  CPPUNIT_ASSERT(obuf != NULL);
  CPPUNIT_ASSERT(ibuf != NULL);
  CPPUNIT_ASSERT(obsize > 0);
  CPPUNIT_ASSERT(ibsize > 0);
  tb->stop();
  tb->wait();
}


void qa_gr_block::t4_throw_connected_mem_blocks()
{
  gr::block_sptr src1(gr::qa::test_custom_buffers_source::make(sizeof(int)));
  gr::block_sptr dst1(gr::qa::test_custom_buffers_sink::make(sizeof(int)));

  gr::top_block_sptr tb(gr::make_top_block("t4"));
  tb->connect(src1, 0, dst1, 0);
  CPPUNIT_ASSERT_THROW_MESSAGE("Connecting a block-allocated output to a block-allocated input should throw exception",
                               tb->start(), std::runtime_error);
}

void qa_gr_block::t5_custom_allocator_input()
{
  gr::block_sptr src1(gr::blocks::null_source::make(sizeof(int)));
  gr::block_sptr dst1(gr::qa::test_custom_buffers_sink::make(sizeof(int)));

  gr::top_block_sptr tb(gr::make_top_block("t5"));
  tb->connect(src1, 0, dst1, 0);
  tb->start();
  const char *obuf = src1->detail()->output(0)->base();
  int obsize = src1->detail()->output(0)->bufsize();
  const char *ibuf = dst1->detail()->input(0)->buffer()->base();
  int ibsize = dst1->detail()->input(0)->buffer()->bufsize();
  CPPUNIT_ASSERT(obuf != NULL);
  CPPUNIT_ASSERT(ibuf != NULL);
  CPPUNIT_ASSERT(obsize > 0);
  CPPUNIT_ASSERT(ibsize > 0);
  tb->stop();
  tb->wait();
}
