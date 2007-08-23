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
#include <config.h>
#endif

#include <qa_gr_flowgraph.h>
#include <gr_flowgraph.h>
#include <gr_nop.h>
#include <gr_null_source.h>
#include <gr_null_sink.h>

void qa_gr_flowgraph::t0()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  CPPUNIT_ASSERT(fg);
}

void qa_gr_flowgraph::t1_connect()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
}

void qa_gr_flowgraph::t2_connect_invalid_src_port_neg()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, -1, nop2, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t3_connect_src_port_exceeds()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(src, 1, dst, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t4_connect_invalid_dst_port_neg()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, 0, nop2, -1), std::invalid_argument);
}

void qa_gr_flowgraph::t5_connect_dst_port_exceeds()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(src, 0, dst, 1), std::invalid_argument);
}

void qa_gr_flowgraph::t6_connect_dst_in_use()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr src1 = gr_make_null_source(sizeof(int));
  gr_block_sptr src2 = gr_make_null_source(sizeof(int));
  gr_block_sptr dst = gr_make_null_sink(sizeof(int));

  fg->connect(src1, 0, dst, 0);
  CPPUNIT_ASSERT_THROW(fg->connect(src2, 0, dst, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t7_connect_one_src_two_dst()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr src = gr_make_null_source(sizeof(int));
  gr_block_sptr dst1 = gr_make_null_sink(sizeof(int));
  gr_block_sptr dst2 = gr_make_null_sink(sizeof(int));

  fg->connect(src, 0, dst1, 0);
  fg->connect(src, 0, dst2, 0);
}

void qa_gr_flowgraph::t8_connect_type_mismatch()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(char));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  CPPUNIT_ASSERT_THROW(fg->connect(nop1, 0, nop2, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t9_disconnect()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->disconnect(nop1, 0, nop2, 0);
}

void qa_gr_flowgraph::t10_disconnect_unconnected_block()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));
  gr_block_sptr nop3 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  CPPUNIT_ASSERT_THROW(fg->disconnect(nop1, 0, nop3, 0), std::invalid_argument);
}

void qa_gr_flowgraph::t11_disconnect_unconnected_port()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  CPPUNIT_ASSERT_THROW(fg->disconnect(nop1, 0, nop2, 1), std::invalid_argument);
}

void qa_gr_flowgraph::t12_validate()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->validate();
}

void qa_gr_flowgraph::t13_validate_missing_input_assignment()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 0, nop2, 2);
  CPPUNIT_ASSERT_THROW(fg->validate(), std::runtime_error);
}

void qa_gr_flowgraph::t14_validate_missing_output_assignment()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);
  fg->connect(nop1, 2, nop2, 1);
  CPPUNIT_ASSERT_THROW(fg->validate(), std::runtime_error);
}

void qa_gr_flowgraph::t15_clear()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop1 = gr_make_nop(sizeof(int));
  gr_block_sptr nop2 = gr_make_nop(sizeof(int));

  fg->connect(nop1, 0, nop2, 0);

  CPPUNIT_ASSERT(fg->edges().size() == 1);
  CPPUNIT_ASSERT(fg->calc_used_blocks().size() == 2);

  fg->clear();

  CPPUNIT_ASSERT(fg->edges().size() == 0);
  CPPUNIT_ASSERT(fg->calc_used_blocks().size() == 0);
}

void qa_gr_flowgraph::t16_partition()
{
  gr_flowgraph_sptr fg = gr_make_flowgraph();

  gr_block_sptr nop11 = gr_make_nop(sizeof(int));
  gr_block_sptr nop12 = gr_make_nop(sizeof(int));
  gr_block_sptr nop13 = gr_make_nop(sizeof(int));
  gr_block_sptr nop14 = gr_make_nop(sizeof(int));

  gr_block_sptr nop21 = gr_make_nop(sizeof(int));
  gr_block_sptr nop22 = gr_make_nop(sizeof(int));
  gr_block_sptr nop23 = gr_make_nop(sizeof(int));

  gr_block_sptr nop31 = gr_make_nop(sizeof(int));
  gr_block_sptr nop32 = gr_make_nop(sizeof(int));
  
  // Build disjoint graph #1
  fg->connect(nop11, 0, nop12, 0);
  fg->connect(nop12, 0, nop13, 0);
  fg->connect(nop13, 0, nop14, 0);

  // Build disjoint graph #2
  fg->connect(nop21, 0, nop22, 0);
  fg->connect(nop22, 0, nop23, 0);

  // Build disjoint graph #3
  fg->connect(nop31, 0, nop32, 0);

  std::vector<gr_basic_block_vector_t> graphs = fg->partition();

  CPPUNIT_ASSERT(graphs.size() == 3);
  CPPUNIT_ASSERT(graphs[0].size() == 4);
  CPPUNIT_ASSERT(graphs[1].size() == 3);
  CPPUNIT_ASSERT(graphs[2].size() == 2);
}
