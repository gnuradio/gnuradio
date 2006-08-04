/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "encoder_convolutional.h"
#include <qa_encoder_convolutional_ic1_ic1.h>
#include <cppunit/TestAssert.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>

void
qa_encoder_convolutional_ic1_ic1::do_encoder_check
(int test_n,
 const char** c_in,
 const char** c_res,
 int n_input_items,
 int n_output_items,
 int block_size_bits,
 int n_code_inputs,
 int n_code_outputs,
 const int* code_generators,
 const int* code_feedback)
{
  std::vector<int> t_code_generators;
  t_code_generators.assign (n_code_inputs * n_code_outputs, 0);
  for (int m = 0; m < n_code_inputs * n_code_outputs; m++)
    t_code_generators[m] = code_generators[m];

  encoder_convolutional* t_encoder;

  if (code_feedback) {
    std::vector<int> t_code_feedback;
    t_code_feedback.assign (n_code_inputs * n_code_outputs, 0);
    for (int m = 0; m < n_code_inputs * n_code_outputs; m++)
      t_code_feedback[m] = code_feedback[m];

    t_encoder = new encoder_convolutional
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       t_code_generators,
       t_code_feedback);
  } else {
    t_encoder = new encoder_convolutional
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       t_code_generators);
  }

  code_input_ic1l* t_c_in = new code_input_ic1l (n_code_inputs);
  t_c_in->set_buffer ((void**) c_in, n_input_items);

  char** t_out = new char*[n_code_outputs];
  for (int m = 0; m < n_code_outputs; m++) {
    t_out[m] = new char[n_output_items];
  }

  code_output_ic1l* t_c_out = new code_output_ic1l (n_code_outputs);
  t_c_out->set_buffer ((void**) t_out, n_output_items);

  t_encoder->encode (t_c_in, t_c_out, n_output_items);

  for (int m = 0; m < n_code_outputs; m++) {
    for (int n = 0; n < n_output_items; n++) {
      if (c_res[m][n] != t_out[m][n]) {
	std::cout << "Test " << test_n << ": Item [" << m <<
	  "][" << n << "] not equal: des = " << (int)(c_res[m][n]) <<
	  ", act = " << (int)(t_out[m][n]) << "\n";
      }
      CPPUNIT_ASSERT_EQUAL (c_res[m][n], t_out[m][n]);
    }
  }

  delete t_c_out;
  t_c_out = 0;
  delete t_c_in;
  t_c_in = 0;
  delete t_encoder;
  t_encoder = 0;

  for (int m = 0; m < n_code_outputs; m++) {
    delete [] t_out[m];
    t_out[m] = 0;
  }
  delete [] t_out;
  t_out = 0;
}

// TEST 0
//
// checking for SOAI realization (implicitely)
// no feedback, no termination

const static int t0_code_generator[6] = {1, 0, 5, 0, 1, 6};
const static int t0_n_code_inputs = 3;
const static int t0_n_code_outputs = 2;
const static int t0_n_input_items = 10;
const static int t0_n_output_items = 10;

const static char t0_in_0[t0_n_input_items] =
  {0, 1, 0, 0, 1, 0, 1, 0, 0, 0};
const static char t0_in_1[t0_n_input_items] =
  {0, 1, 0, 0, 0, 1, 1, 0, 0, 0};
const static char t0_in_2[t0_n_input_items] =
  {0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
const static char* t0_in[t0_n_code_inputs] =
  {t0_in_0, t0_in_1, t0_in_2};

const static char t0_res_0[t0_n_output_items] =
  {0, 1, 1, 1, 1, 0, 1, 1, 1, 0};
const static char t0_res_1[t0_n_output_items] =
  {0, 1, 0, 1, 0, 1, 1, 0, 1, 0};
const static char* t0_res[t0_n_code_outputs] =
  {t0_res_0, t0_res_1};

void
qa_encoder_convolutional_ic1_ic1::t0
()
{
  do_encoder_check (0, (const char**) t0_in, (const char**) t0_res,
		    t0_n_input_items, t0_n_output_items, 100,
		    t0_n_code_inputs, t0_n_code_outputs,
		    (const int*) t0_code_generator);
}

// TEST 1
//
// checking for SIAO realization (implicitely)
// no feedback, no termination

const static int t1_code_generator[6] = {1, 0, 0, 1, 5, 6};
const static int t1_n_code_inputs = 2;
const static int t1_n_code_outputs = 3;
const static int t1_n_input_items = 9;
const static int t1_n_output_items = 9;

const static char t1_in_0[t1_n_input_items] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t1_in_1[t1_n_input_items] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char* t1_in[t1_n_code_inputs] =
  {t1_in_0, t1_in_1};

const static char t1_res_0[t1_n_output_items] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t1_res_1[t1_n_output_items] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char t1_res_2[t1_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 1, 0};
const static char* t1_res[t1_n_code_outputs] =
  {t1_res_0, t1_res_1, t1_res_2};

void
qa_encoder_convolutional_ic1_ic1::t1
()
{
  do_encoder_check (1, (const char**) t1_in, (const char**) t1_res,
		    t1_n_input_items, t1_n_output_items, 100,
		    t1_n_code_inputs, t1_n_code_outputs,
		    (const int*) t1_code_generator);
}

// TEST 2
//
// checking for SIAO realization (implicitely)
// with same feedback, no termination

const static int t2_code_generator[6] = {1, 0, 0, 1, 5, 6};
const static int t2_code_feedback[6] = {0, 0, 0, 0, 7, 7};
const static int t2_n_code_inputs = 2;
const static int t2_n_code_outputs = 3;
const static int t2_n_input_items = 19;
const static int t2_n_output_items = 19;

const static char t2_in_0[t2_n_input_items] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t2_in_1[t2_n_input_items] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char* t2_in[t2_n_code_inputs] =
  {t2_in_0, t2_in_1};

const static char t2_res_0[t2_n_output_items] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t2_res_1[t2_n_output_items] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char t2_res_2[t2_n_output_items] =
  {0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0};
const static char* t2_res[t2_n_code_outputs] =
  {t2_res_0, t2_res_1, t2_res_2};

void
qa_encoder_convolutional_ic1_ic1::t2
()
{
  do_encoder_check (2, (const char**) t2_in, (const char**) t2_res,
		    t2_n_input_items, t2_n_output_items, 100,
		    t2_n_code_inputs, t2_n_code_outputs,
		    (const int*) t2_code_generator,
		    (const int*) t2_code_feedback);
}

// TEST 3
//
// checking for SOAI realization (implicitely)
// with same feedback, no termination

const static int t3_code_generator[6] = {1, 0, 5, 0, 1, 6};
const static int t3_code_feedback[6] = {0, 0, 7, 0, 0, 7};
const static int t3_n_code_inputs = 3;
const static int t3_n_code_outputs = 2;
const static int t3_n_input_items = 17;
const static int t3_n_output_items = 17;

const static char t3_in_0[t3_n_input_items] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t3_in_1[t3_n_input_items] =
  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static char t3_in_2[t3_n_input_items] =
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
const static char* t3_in[t3_n_code_inputs] =
  {t3_in_0, t3_in_1, t3_in_2};

const static char t3_res_0[t3_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t3_res_1[t3_n_output_items] =
  {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0};
const static char* t3_res[t3_n_code_outputs] =
  {t3_res_0, t3_res_1};

void
qa_encoder_convolutional_ic1_ic1::t3
()
{
  do_encoder_check (3, (const char**) t3_in, (const char**) t3_res,
		    t3_n_input_items, t3_n_output_items, 100,
		    t3_n_code_inputs, t3_n_code_outputs,
		    (const int*) t3_code_generator,
		    (const int*) t3_code_feedback);
}

// TEST 4
//
// checking for SIAO realization (implicitely),
// with different feedbacks, no termination

const static int t4_code_generator[6] = {1, 4, 0, 3, 1, 6};
const static int t4_code_feedback[6] = {0, 7, 0, 5, 0, 5};
const static int t4_n_code_inputs = 2;
const static int t4_n_code_outputs = 3;
const static int t4_n_input_items = 20;
const static int t4_n_output_items = 20;

const static char t4_in_0[t4_n_input_items] =
  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0};
const static char t4_in_1[t4_n_input_items] =
  {0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t4_in[t4_n_code_inputs] =
  {t4_in_0, t4_in_1};

const static char t4_res_0[t4_n_output_items] =
  {0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0};
const static char t4_res_1[t4_n_output_items] =
  {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0};
const static char t4_res_2[t4_n_output_items] =
  {0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0};
const static char* t4_res[t4_n_code_outputs] =
  {t4_res_0, t4_res_1, t4_res_2};

void
qa_encoder_convolutional_ic1_ic1::t4
()
{
  do_encoder_check (4, (const char**) t4_in, (const char**) t4_res,
		    t4_n_input_items, t4_n_output_items, 100,
		    t4_n_code_inputs, t4_n_code_outputs,
		    (const int*) t4_code_generator,
		    (const int*) t4_code_feedback);
}

// TEST 5
//
// checking for SOAI realization (implicitely),
// with different feedbacks, no termination

const static int t5_code_generator[6] = {1, 0, 0, 1, 5, 7};
const static int t5_code_feedback[6] = {0, 0, 0, 0, 7, 3};
const static int t5_n_code_inputs = 2;
const static int t5_n_code_outputs = 3;
const static int t5_n_input_items = 19;
const static int t5_n_output_items = 19;

const static char t5_in_0[t5_n_input_items] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t5_in_1[t5_n_input_items] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char* t5_in[t5_n_code_inputs] =
  {t5_in_0, t5_in_1};

const static char t5_res_0[t5_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t5_res_1[t5_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t5_res_2[t5_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char* t5_res[t5_n_code_outputs] =
  {t5_res_0, t5_res_1, t5_res_2};

void
qa_encoder_convolutional_ic1_ic1::t5
()
{
#if 0
  do_encoder_check (5, (const char**) t5_in, (const char**) t5_res,
		    t5_n_input_items, t5_n_output_items, 100,
		    t5_n_code_inputs, t5_n_code_outputs,
		    (const int*) t5_code_generator);
#endif
}

// TEST 6
//
// checking for termination, no feedback

const static int t6_code_generator[6] = {1, 0, 5, 0, 1, 6};
const static int t6_n_code_inputs = 3;
const static int t6_n_code_outputs = 2;
const static int t6_n_input_items = 6;
const static int t6_n_output_items = 8;

const static char t6_in_0[t6_n_input_items] =
  {0, 1, 0, 0, 1, 0};
const static char t6_in_1[t6_n_input_items] =
  {0, 1, 0, 0, 0, 0};
const static char t6_in_2[t6_n_input_items] =
  {0, 0, 1, 1, 1, 0};
const static char* t6_in[t6_n_code_inputs] =
  {t6_in_0, t6_in_1, t6_in_2};

const static char t6_res_0[t6_n_output_items] =
  {0, 1, 1, 1, 1, 1, 1, 0};
const static char t6_res_1[t6_n_output_items] =
  {0, 1, 0, 1, 0, 0, 1, 0};
const static char* t6_res[t6_n_code_outputs] =
  {t6_res_0, t6_res_1};

void
qa_encoder_convolutional_ic1_ic1::t6
()
{
  do_encoder_check (6, (const char**) t6_in, (const char**) t6_res,
		    t6_n_input_items, t6_n_output_items, 5, t6_n_code_inputs,
		    t6_n_code_outputs, (const int*) t6_code_generator);
}

// TEST 7
//
// checking for termination, with same feedback

const static int t7_code_generator[6] = {1, 0, 5, 0, 1, 6};
const static int t7_code_feedback[6] = {0, 0, 7, 0, 0, 7};
const static int t7_n_code_inputs = 3;
const static int t7_n_code_outputs = 2;
const static int t7_n_input_items = 17;
const static int t7_n_output_items = 17;

const static char t7_in_0[t7_n_input_items] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t7_in_1[t7_n_input_items] =
  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static char t7_in_2[t7_n_input_items] =
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
const static char* t7_in[t7_n_code_inputs] =
  {t7_in_0, t7_in_1, t7_in_2};

const static char t7_res_0[t7_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t7_res_1[t7_n_output_items] =
  {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0};
const static char* t7_res[t7_n_code_outputs] =
  {t7_res_0, t7_res_1};

void
qa_encoder_convolutional_ic1_ic1::t7
()
{
#if 0
  do_encoder_check (7, (const char**) t7_in, (const char**) t7_res,
		    t7_n_input_items, t7_n_output_items, 5, t7_n_code_inputs,
		    t7_n_code_outputs, (const int*) t7_code_generator,
		    (const int*) t7_code_feedback);
#endif
}

// TEST 8
//
// checking for termination, with different feedback

const static int t8_code_generator[6] = {1, 0, 5, 0, 1, 6};
const static int t8_code_feedback[6] = {0, 0, 7, 0, 0, 3};
const static int t8_n_code_inputs = 3;
const static int t8_n_code_outputs = 2;
const static int t8_n_input_items = 17;
const static int t8_n_output_items = 17;

const static char t8_in_0[t8_n_input_items] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t8_in_1[t8_n_input_items] =
  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static char t8_in_2[t8_n_input_items] =
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
const static char* t8_in[t8_n_code_inputs] =
  {t8_in_0, t8_in_1, t8_in_2};

const static char t8_res_0[t8_n_output_items] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t8_res_1[t8_n_output_items] =
  {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0};
const static char* t8_res[t8_n_code_outputs] =
  {t8_res_0, t8_res_1};

void
qa_encoder_convolutional_ic1_ic1::t8
()
{
#if 0
  do_encoder_check (8, (const char**) t8_in, (const char**) t8_res,
		    t8_n_input_items, t8_n_output_items, 5, t8_n_code_inputs,
		    t8_n_code_outputs, (const int*) t8_code_generator,
		    (const int*) t8_code_feedback);
#endif
}
