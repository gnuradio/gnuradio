/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "qa_square_ff.h"
#include <cppunit/TestAssert.h>

#include <vector>
#include <gr_top_block.h>
#include <gr_vector_source_f.h>
#include <gr_vector_sink_f.h>
#include <howto/square_ff.h>

namespace gr {
  namespace howto {
    
    void
    qa_square_ff::t1()
    {
      std::vector<float> data(5,0);
      std::vector<float> result(5,0);
      std::vector<float> output(5,0);
      for(size_t i = 0; i < data.size(); i++) {
	data[i] = i;
	result[i] = i*i;
      }
      
      gr_top_block_sptr tb = gr_make_top_block("dial_tone");
      gr_vector_source_f_sptr src = gr_make_vector_source_f(data);
      gr_vector_sink_f_sptr snk = gr_make_vector_sink_f();

      square_ff::sptr op = square_ff::make();
      
      tb->connect(src, 0, op, 0);
      tb->connect(op, 0, snk, 0);
      tb->run();

      output = snk->data();
      
      for(size_t i = 0; i < data.size(); i++) {
	CPPUNIT_ASSERT_DOUBLES_EQUAL(output[i], result[i], 0.0001);
      }
    }

  } /* namespace howto */
} /* namespace gr */
