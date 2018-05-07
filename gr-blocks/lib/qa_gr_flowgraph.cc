/* -*- c++ -*- */
/*
 * Copyright 2007,2013 Free Software Foundation, Inc.
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

#include <gnuradio/flowgraph.h>
#include <gnuradio/blocks/nop.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <boost/test/unit_test.hpp>

namespace gr {
  namespace blocks {
    class null_qa_source : virtual public sync_block {
      public:
      typedef boost::shared_ptr<null_qa_source> sptr;
      static sptr make(size_t sizeof_stream_item);
    };
    class null_source_qa_impl : public null_qa_source
    {
      public:
      null_source_qa_impl(size_t sizeof_stream_item)
              : sync_block("null_source", io_signature::make(0, 0, 0), io_signature::make(1, 1, sizeof_stream_item)) {}
      ~null_source_qa_impl(){}

      int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) {
        void *optr;
        for(size_t n = 0; n < input_items.size(); n++) {
          optr = (void*)output_items[n];
          memset(optr, 0, noutput_items * output_signature()->sizeof_stream_item(n));
        }
        return noutput_items;
      }
    };
    null_qa_source::sptr null_qa_source::make(size_t sizeof_stream_item)
    {
      return gnuradio::get_initial_sptr(new null_source_qa_impl(sizeof_stream_item));
    }
    class null_qa_sink : virtual public sync_block
    {
      public:
      typedef boost::shared_ptr<null_qa_sink> sptr;
      static sptr make(size_t sizeof_stream_item);
    };
    class null_sink_qa_impl : public null_qa_sink {
      public:
      null_sink_qa_impl(size_t sizeof_stream_item)
              : sync_block("null_qa_sink", io_signature::make(1, 1, sizeof_stream_item), io_signature::make(0, 0, 0)) {}
      ~null_sink_qa_impl() {}
      int work(int noutput_items, gr_vector_const_void_star &input_items, gr_vector_void_star &output_items) {
        return noutput_items;
      }
    };
    null_qa_sink::sptr null_qa_sink::make(size_t sizeof_stream_item) {
      return gnuradio::get_initial_sptr(new null_sink_qa_impl(sizeof_stream_item));
    }
  } /* namespace blocks */
} /* namespace gr */

BOOST_AUTO_TEST_CASE(t0)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  BOOST_REQUIRE(fg);
}

BOOST_AUTO_TEST_CASE(t1_connect)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
}

BOOST_AUTO_TEST_CASE(t2_connect_invalid_src_port_neg)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  BOOST_REQUIRE_THROW(fg->connect(nop1, -1, nop2, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t3_connect_src_port_exceeds)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_qa_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  BOOST_REQUIRE_THROW(fg->connect(src, 1, dst, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t4_connect_invalid_dst_port_neg)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  BOOST_REQUIRE_THROW(fg->connect(nop1, 0, nop2, -1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t5_connect_dst_port_exceeds)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_qa_sink::make(sizeof(int));

  BOOST_REQUIRE_THROW(fg->connect(src, 0, dst, 1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t6_connect_dst_in_use)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src1 = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr src2 = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  fg->connect(src1, 0, dst, 0);
  BOOST_REQUIRE_THROW(fg->connect(src2, 0, dst, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t7_connect_one_src_two_dst)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst1 = gr::blocks::null_sink::make(sizeof(int));
  gr::block_sptr dst2 = gr::blocks::null_sink::make(sizeof(int));

  fg->connect(src, 0, dst1, 0);
  fg->connect(src, 0, dst2, 0);
}

BOOST_AUTO_TEST_CASE(t8_connect_type_mismatch)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(char));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  BOOST_REQUIRE_THROW(fg->connect(nop1, 0, nop2, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t9_disconnect)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->disconnect(nop1, 0, nop2, 0);
}

BOOST_AUTO_TEST_CASE(t10_disconnect_unconnected_block)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop3 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  BOOST_REQUIRE_THROW(fg->disconnect(nop1, 0, nop3, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t11_disconnect_unconnected_port)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  BOOST_REQUIRE_THROW(fg->disconnect(nop1, 0, nop2, 1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(t12_validate)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->validate();
}

BOOST_AUTO_TEST_CASE(t13_validate_missing_input_assignment)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 0, nop2, 2);
  BOOST_REQUIRE_THROW(fg->validate(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(t14_validate_missing_output_assignment)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 2, nop2, 1);
  BOOST_REQUIRE_THROW(fg->validate(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(t15_clear)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);

  BOOST_REQUIRE(fg->edges().size() == 1);
  BOOST_REQUIRE(fg->calc_used_blocks().size() == 2);

  fg->clear();

  BOOST_REQUIRE(fg->edges().size() == 0);
  BOOST_REQUIRE(fg->calc_used_blocks().size() == 0);
}

BOOST_AUTO_TEST_CASE(t16_partition)
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop11 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop12 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop13 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop14 = gr::blocks::nop::make(sizeof(int));

  gr::block_sptr nop21 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop22 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop23 = gr::blocks::nop::make(sizeof(int));

  gr::block_sptr nop31 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop32 = gr::blocks::nop::make(sizeof(int));

  // Build disjoint graph #1
  fg->connect(nop11, 0, nop12, 0);
  fg->connect(nop12, 0, nop13, 0);
  fg->connect(nop13, 0, nop14, 0);

  // Build disjoint graph #2
  fg->connect(nop21, 0, nop22, 0);
  fg->connect(nop22, 0, nop23, 0);

  // Build disjoint graph #3
  fg->connect(nop31, 0, nop32, 0);

  std::vector<gr::basic_block_vector_t> graphs = fg->partition();

  BOOST_REQUIRE(graphs.size() == 3);
  BOOST_REQUIRE(graphs[0].size() == 4);
  BOOST_REQUIRE(graphs[1].size() == 3);
  BOOST_REQUIRE(graphs[2].size() == 2);
}
