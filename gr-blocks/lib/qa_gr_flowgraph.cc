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

#include <qa_gr_flowgraph.h>
#include <gnuradio/flowgraph.h>
#include <gnuradio/blocks/nop.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>

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

void qa_gr_flowgraph::t0()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  CPPUNIT_ASSERT(fg);
}

void qa_gr_flowgraph::t1_connect()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
}

void qa_gr_flowgraph::t2_connect_invalid_src_port_neg()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, -1, nop2, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t3_connect_src_port_exceeds()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_qa_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(src, 1, dst, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t4_connect_invalid_dst_port_neg()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, 0, nop2, -1), std::invalid_argument);
}

void qa_gr_flowgraph::t5_connect_dst_port_exceeds()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_qa_sink::make(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(src, 0, dst, 1), std::invalid_argument);
}

void qa_gr_flowgraph::t6_connect_dst_in_use()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src1 = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr src2 = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst = gr::blocks::null_sink::make(sizeof(int));

  fg->connect(src1, 0, dst, 0);
  CPPUNIT_ASSERT_THROW(fg->connect(src2, 0, dst, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t7_connect_one_src_two_dst()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr src = gr::blocks::null_source::make(sizeof(int));
  gr::block_sptr dst1 = gr::blocks::null_sink::make(sizeof(int));
  gr::block_sptr dst2 = gr::blocks::null_sink::make(sizeof(int));

  fg->connect(src, 0, dst1, 0);
  fg->connect(src, 0, dst2, 0);
}

void qa_gr_flowgraph::t8_connect_type_mismatch()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(char));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, 0, nop2, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t9_disconnect()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->disconnect(nop1, 0, nop2, 0);
}

void qa_gr_flowgraph::t10_disconnect_unconnected_block()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop3 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  CPPUNIT_ASSERT_THROW(fg->disconnect(nop1, 0, nop3, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t11_disconnect_unconnected_port()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  CPPUNIT_ASSERT_THROW(fg->disconnect(nop1, 0, nop2, 1), std::invalid_argument);
}

void qa_gr_flowgraph::t12_validate()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->validate();
}

void qa_gr_flowgraph::t13_validate_missing_input_assignment()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 0, nop2, 2);
  CPPUNIT_ASSERT_THROW(fg->validate(), std::runtime_error);
}

void qa_gr_flowgraph::t14_validate_missing_output_assignment()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 2, nop2, 1);
  CPPUNIT_ASSERT_THROW(fg->validate(), std::runtime_error);
}

void qa_gr_flowgraph::t15_clear()
{
  gr::flowgraph_sptr fg = gr::make_flowgraph();

  gr::block_sptr nop1 = gr::blocks::nop::make(sizeof(int));
  gr::block_sptr nop2 = gr::blocks::nop::make(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);

  CPPUNIT_ASSERT(fg->edges().size() == 1);
  CPPUNIT_ASSERT(fg->calc_used_blocks().size() == 2);

  fg->clear();

  CPPUNIT_ASSERT(fg->edges().size() == 0);
  CPPUNIT_ASSERT(fg->calc_used_blocks().size() == 0);
}

void qa_gr_flowgraph::t16_partition()
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

  CPPUNIT_ASSERT(graphs.size() == 3);
  CPPUNIT_ASSERT(graphs[0].size() == 4);
  CPPUNIT_ASSERT(graphs[1].size() == 3);
  CPPUNIT_ASSERT(graphs[2].size() == 2);
}
