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

#define DO_PRINT_DEBUG 0

// test number counter

static size_t g_t_num = 0;

void
qa_encoder_convolutional_ic1_ic1::do_encoder_check
(bool use_encode_in_or_out,
 const char** c_in,
 const char** c_res,
 const size_t n_io_items,
 const size_t* n_input_items,
 const size_t* n_output_items,
 const size_t block_size_bits,
 const size_t n_code_inputs,
 const size_t n_code_outputs,
 const int* code_generators,
 const int* code_feedback,
 const bool do_termination,
 const size_t start_state,
 const size_t term_state,
 const int encode_soai)
{
  if (DO_PRINT_DEBUG)
    std::cout << "Starting Test " << g_t_num << "\n";

  std::vector<int> t_code_generators;
  t_code_generators.assign (n_code_inputs * n_code_outputs, 0);
  for (size_t m = 0; m < (n_code_inputs * n_code_outputs); m++)
    t_code_generators[m] = code_generators[m];

  encoder_convolutional* t_encoder;

  if (code_feedback) {
    std::vector<int> t_code_feedback;
    t_code_feedback.assign (n_code_inputs * n_code_outputs, 0);
    for (size_t m = 0; m < (n_code_inputs * n_code_outputs); m++)
      t_code_feedback[m] = code_feedback[m];

    t_encoder = new encoder_convolutional
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       t_code_generators,
       t_code_feedback,
       do_termination,
       start_state,
       term_state);
  } else {
    t_encoder = new encoder_convolutional
      (block_size_bits,
       n_code_inputs,
       n_code_outputs,
       t_code_generators,
       do_termination,
       start_state,
       term_state);
  }

  size_t t_total_n_input_items = 0;
  size_t t_total_n_output_items = 0;
  for (size_t m = 0; m < n_io_items; m++) {
    t_total_n_input_items += n_input_items[m];
    t_total_n_output_items += n_output_items[m];
  }

  code_input_ic1l* t_c_in = new code_input_ic1l (n_code_inputs);
  t_c_in->set_buffer ((void**) c_in, t_total_n_input_items);

  char** t_out = new char*[n_code_outputs];
  for (size_t m = 0; m < n_code_outputs; m++) {
    t_out[m] = new char[t_total_n_output_items];
  }

  code_output_ic1l* t_c_out = new code_output_ic1l (n_code_outputs);
  t_c_out->set_buffer ((void**) t_out, t_total_n_output_items);

  bool t_errors = false;

  for (size_t m = 0; m < n_io_items; m++) {
    if (use_encode_in_or_out == true) {
      size_t t_n_output_items_used = t_encoder->encode (t_c_in,
							n_input_items[m],
							t_c_out);
      if (t_n_output_items_used != n_output_items[m]) {
	std::cout << "Test " << g_t_num << ": Encode[" << m <<
	  "]{input}: Warning: Number of returned output items (" <<
	  t_n_output_items_used << ") is not as expected (" <<
	  n_output_items[m] << ").\n";
	t_errors = true;
      }
    } else {
      size_t t_n_input_items_used = t_encoder->encode (t_c_in,
						       t_c_out,
						       n_output_items[m]);
      if (t_n_input_items_used != n_input_items[m]) {
	std::cout << "Test " << g_t_num << ": Encode[" << m <<
	  "]{output}: Warning: Number of returned output items (" <<
	  t_n_input_items_used << ") is not as expected (" <<
	  n_input_items[m] << ").\n";
	t_errors = true;
      }
    }
  }

  for (size_t m = 0; m < n_code_outputs; m++) {
    for (size_t n = 0; n < t_total_n_output_items; n++) {
      if (c_res[m][n] != t_out[m][n]) {
	std::cout << "Test " << g_t_num << ": Item [" << m <<
	  "][" << n << "] not equal: des = " << (int)(c_res[m][n]) <<
	  ", act = " << (int)(t_out[m][n]) << "\n";
	t_errors = true;
      }
    }
  }

  if (encode_soai != -1) {
    // verify that the internal realization is correct
    if (encode_soai != t_encoder->do_encode_soai ()) {
      t_errors = true;
    }
  }

  CPPUNIT_ASSERT_EQUAL (t_errors, false);

  delete t_c_out;
  t_c_out = 0;
  delete t_c_in;
  t_c_in = 0;
  delete t_encoder;
  t_encoder = 0;

  for (size_t m = 0; m < n_code_outputs; m++) {
    delete [] t_out[m];
    t_out[m] = 0;
  }
  delete [] t_out;
  t_out = 0;

  // increment the test number

  g_t_num++;
}

// TESTS 0 through 22 use do_encode_in_or_out as false

static const bool g_do_encode_in_or_out_0_22 = false;

// TEST 0
//
// checking for:
// SIAO realization (implicitely)
// start state is 0, no feedback, no termination

const static int t0_code_generator[] = {1, 0, 5, 0, 1, 6};
const static size_t t0_encode_soai = 0;

const static char t0_in_0[] =
  {0, 1, 0, 0, 1, 0, 1, 0, 0, 0};
const static char t0_in_1[] =
  {0, 1, 0, 0, 0, 1, 1, 0, 0, 0};
const static char t0_in_2[] =
  {0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
const static char* t0_in[] =
  {t0_in_0, t0_in_1, t0_in_2};

const static size_t t0_n_input_items = sizeof (t0_in_0);
const static size_t t0_n_code_inputs = sizeof (t0_in) / sizeof (char*);

const static char t0_res_0[] =
  {0, 1, 1, 1, 1, 0, 1, 1, 1, 0};
const static char t0_res_1[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 1, 0};
const static char* t0_res[] =
  {t0_res_0, t0_res_1};

const static size_t t0_n_output_items = sizeof (t0_res_0);
const static size_t t0_n_code_outputs = sizeof (t0_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t0
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t0_in, (const char**) t0_res,
		    1, (const size_t*) &t0_n_input_items, 
		    (const size_t*) &t0_n_output_items, 100,
		    t0_n_code_inputs, t0_n_code_outputs,
		    (const int*) t0_code_generator,
		    0, true, 0, 0, t0_encode_soai);
}

// TEST 1
//
// checking for SIAO realization (implicitely)
// start state is 0, no feedback, no termination

const static int t1_code_generator[] = {1, 0, 0, 1, 5, 6};
const static size_t t1_encode_soai = 1;

const static char t1_in_0[] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t1_in_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char* t1_in[] =
  {t1_in_0, t1_in_1};

const static size_t t1_n_input_items = sizeof (t1_in_0);
const static size_t t1_n_code_inputs = sizeof (t1_in) / sizeof (char*);

const static char t1_res_0[] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t1_res_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char t1_res_2[] =
  {0, 1, 1, 0, 1, 1, 1, 1, 0};
const static char* t1_res[] =
  {t1_res_0, t1_res_1, t1_res_2};

const static size_t t1_n_output_items = sizeof (t1_res_0);
const static size_t t1_n_code_outputs = sizeof (t1_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t1
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t1_in, (const char**) t1_res,
		    1, (const size_t*) &t1_n_input_items, 
		    (const size_t*) &t1_n_output_items, 100,
		    t1_n_code_inputs, t1_n_code_outputs,
		    (const int*) t1_code_generator,
		    0, true, 0, 0, t1_encode_soai);
}

// TEST 2
//
// checking for SOAI realization (implicitely)
// start state is 0, same feedback, no termination

const static int t2_code_generator[] = {1, 0, 0, 1, 5, 6};
const static size_t t2_code_feedback[] = {0, 0, 0, 0, 7, 7};
const static int t2_encode_soai = 1;

const static char t2_in_0[] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t2_in_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char* t2_in[] =
  {t2_in_0, t2_in_1};

const static size_t t2_n_input_items = sizeof (t2_in_0);
const static size_t t2_n_code_inputs = sizeof (t2_in) / sizeof (char*);

const static char t2_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t2_res_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char t2_res_2[] =
  {0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0};
const static char* t2_res[] =
  {t2_res_0, t2_res_1, t2_res_2};

const static size_t t2_n_output_items = sizeof (t2_res_0);
const static size_t t2_n_code_outputs = sizeof (t2_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t2
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t2_in, (const char**) t2_res,
		    1, (const size_t*) &t2_n_input_items, 
		    (const size_t*) &t2_n_output_items, 100,
		    t2_n_code_inputs, t2_n_code_outputs,
		    (const int*) t2_code_generator,
		    (const int*) t2_code_feedback,
		    true, 0, 0, t2_encode_soai);
}

// TEST 3
//
// checking for SIAO realization (implicitely)
// start state is 0, same feedback, no termination

const static int t3_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t3_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static int t3_encode_soai = 0;

const static char t3_in_0[] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t3_in_1[] =
  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static char t3_in_2[] =
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
const static char* t3_in[] =
  {t3_in_0, t3_in_1, t3_in_2};

const static size_t t3_n_input_items = sizeof (t3_in_0);
const static size_t t3_n_code_inputs = sizeof (t3_in) / sizeof (char*);

const static char t3_res_0[] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t3_res_1[] =
  {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0};
const static char* t3_res[] =
  {t3_res_0, t3_res_1};

const static size_t t3_n_output_items = sizeof (t3_res_0);
const static size_t t3_n_code_outputs = sizeof (t3_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t3
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t3_in, (const char**) t3_res,
		    1, (const size_t*) &t3_n_input_items, 
		    (const size_t*) &t3_n_output_items, 100,
		    t3_n_code_inputs, t3_n_code_outputs,
		    (const int*) t3_code_generator,
		    (const int*) t3_code_feedback,
		    true, 0, 0, t3_encode_soai);
}

// TEST 4
//
// checking for SIAO realization (implicitely),
// start state is 0, different feedbacks, no termination

const static int t4_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t4_code_feedback[] = {0, 7, 0, 5, 0, 5};
const static int t4_encode_soai = 0;

const static char t4_in_0[] =
  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0};
const static char t4_in_1[] =
  {0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t4_in[] =
  {t4_in_0, t4_in_1};

const static size_t t4_n_input_items = sizeof (t4_in_0);
const static size_t t4_n_code_inputs = sizeof (t4_in) / sizeof (char*);

const static char t4_res_0[] =
  {0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0};
const static char t4_res_1[] =
  {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0};
const static char t4_res_2[] =
  {0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0};
const static char* t4_res[] =
  {t4_res_0, t4_res_1, t4_res_2};

const static size_t t4_n_output_items = sizeof (t4_res_0);
const static size_t t4_n_code_outputs = sizeof (t4_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t4
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t4_in, (const char**) t4_res,
		    1, (const size_t*) &t4_n_input_items, 
		    (const size_t*) &t4_n_output_items, 100,
		    t4_n_code_inputs, t4_n_code_outputs,
		    (const int*) t4_code_generator,
		    (const int*) t4_code_feedback,
		    true, 0, 0, t4_encode_soai);
}

// TEST 5
//
// checking for SOAI realization (implicitely),
// with different feedbacks, no termination

const static int t5_code_generator[] = {1, 0, 1, 4, 3, 6};
const static int t5_code_feedback[] = {0, 0, 0, 5, 7, 7};
const static int t5_encode_soai = 1;

const static char t5_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 0, 1};
const static char t5_in_1[] =
  {0, 0, 1, 0, 1, 1, 0, 0, 1};
const static char t5_in_2[] =
  {0, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t5_in[] =
  {t5_in_0, t5_in_1, t5_in_2};

const static size_t t5_n_input_items = sizeof (t5_in_0);
const static size_t t5_n_code_inputs = sizeof (t5_in) / sizeof (char*);

const static char t5_res_0[] =
  {0, 1, 0, 1, 0, 1, 0, 0, 0};
const static char t5_res_1[] =
  {0, 0, 1, 1, 1, 1, 0, 0, 1};
const static char* t5_res[] =
  {t5_res_0, t5_res_1};

const static size_t t5_n_output_items = sizeof (t5_res_0);
const static size_t t5_n_code_outputs = sizeof (t5_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t5
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t5_in, (const char**) t5_res,
		    1, (const size_t*) &t5_n_input_items,
		    (const size_t*) &t5_n_output_items, 100,
		    t5_n_code_inputs, t5_n_code_outputs,
		    (const int*) t5_code_generator,
		    (const int*) t5_code_feedback, true,
		    0, 0, t5_encode_soai);
}

// TEST 6
//
// checking for:
// start state is 0, no feedback, termination to 0 state

const static int t6_code_generator[] = {1, 0, 5, 0, 1, 6};

const static char t6_in_0[] =
  {0, 1, 0, 0, 1, 0};
const static char t6_in_1[] =
  {0, 1, 0, 0, 0, 0};
const static char t6_in_2[] =
  {0, 0, 1, 1, 1, 0};
const static char* t6_in[] =
  {t6_in_0, t6_in_1, t6_in_2};

const static size_t t6_n_input_items = sizeof (t6_in_0);
const static size_t t6_n_code_inputs = sizeof (t6_in) / sizeof (char*);

const static char t6_res_0[] =
  {0, 1, 1, 1, 1, 1, 1, 0};
const static char t6_res_1[] =
  {0, 1, 0, 1, 0, 0, 1, 0};
const static char* t6_res[] =
  {t6_res_0, t6_res_1};

const static size_t t6_n_output_items = sizeof (t6_res_0);
const static size_t t6_n_code_outputs = sizeof (t6_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t6
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t6_in, (const char**) t6_res,
		    1, (const size_t*) &t6_n_input_items, 
		    (const size_t*) &t6_n_output_items, 5, t6_n_code_inputs,
		    t6_n_code_outputs, (const int*) t6_code_generator);
}

// TEST 7
//
// checking for:
// start state is 0, same feedback, termination to 0 state
// # of termination bits = 2

const static int t7_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t7_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t7_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t7_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t7_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t7_in[] =
  {t7_in_0, t7_in_1, t7_in_2};

const static size_t t7_n_input_items = sizeof (t7_in_0);
const static size_t t7_n_code_inputs = sizeof (t7_in) / sizeof (char*);

const static char t7_res_0[] =
  {0, 1, 1, 0, 1, 0, 1, 0, 1};
const static char t7_res_1[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 0};
const static char* t7_res[] =
  {t7_res_0, t7_res_1};

const static size_t t7_n_output_items = sizeof (t7_res_0);
const static size_t t7_n_code_outputs = sizeof (t7_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t7
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t7_in, (const char**) t7_res,
		    1, (const size_t*) &t7_n_input_items, 
		    (const size_t*) &t7_n_output_items, 5, t7_n_code_inputs,
		    t7_n_code_outputs, (const int*) t7_code_generator,
		    (const int*) t7_code_feedback);
}

// TEST 8
//
// checking for:
// state state is 0, different feedbacks, termination to 0 state
// # of term bits will be 4

const static int t8_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t8_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t8_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t8_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t8_in[] =
  {t8_in_0, t8_in_1};

const static size_t t8_n_input_items = sizeof (t8_in_0);
const static size_t t8_n_code_inputs = sizeof (t8_in) / sizeof (char*);

const static char t8_res_0[] =
  {0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1};
const static char t8_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t8_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0};
const static char* t8_res[] =
  {t8_res_0, t8_res_1, t8_res_2};

const static size_t t8_n_output_items = sizeof (t8_res_0);
const static size_t t8_n_code_outputs = sizeof (t8_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t8
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t8_in, (const char**) t8_res,
		    1, (const size_t*) &t8_n_input_items, 
		    (const size_t*) &t8_n_output_items, 6, t8_n_code_inputs,
		    t8_n_code_outputs, (const int*) t8_code_generator,
		    (const int*) t8_code_feedback);
}

// TEST 9
//
// checking for:
// start state is 0, different feedbacks, termination to non-0 state
// # of term bits will be 2

const static int t9_code_generator[] = {1, 0, 1, 4, 3, 6};
const static int t9_code_feedback[] = {0, 0, 0, 5, 7, 7};
const static size_t t9_term_state = 4;

const static char t9_in_0[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t9_in_1[] =
  {0, 0, 1, 0, 1, 0, 1};
const static char t9_in_2[] =
  {0, 0, 0, 1, 0, 0, 1};
const static char* t9_in[] =
  {t9_in_0, t9_in_1, t9_in_2};

const static size_t t9_n_input_items = sizeof (t9_in_0);
const static size_t t9_n_code_inputs = sizeof (t9_in) / sizeof (char*);

const static char t9_res_0[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 0};
const static char t9_res_1[] =
  {0, 0, 1, 1, 1, 1, 1, 0, 1};
const static char* t9_res[] =
  {t9_res_0, t9_res_1};

const static size_t t9_n_output_items = sizeof (t9_res_0);
const static size_t t9_n_code_outputs = sizeof (t9_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t9
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t9_in, (const char**) t9_res,
		    1, (const size_t*) &t9_n_input_items,
		    (const size_t*) &t9_n_output_items, 5, t9_n_code_inputs,
		    t9_n_code_outputs, (const int*) t9_code_generator,
		    (const int*) t9_code_feedback, true, 0, t9_term_state);
}

// TEST 10
//
// checking for termination to non-0 state, no feedback

const static int t10_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t10_term_state = 2;

const static char t10_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t10_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t10_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t10_in[] =
  {t10_in_0, t10_in_1, t10_in_2};

const static size_t t10_n_input_items = sizeof (t10_in_0);
const static size_t t10_n_code_inputs = sizeof (t10_in) / sizeof (char*);

const static char t10_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
const static char t10_res_1[] =
  {0, 1, 1, 1, 0, 0, 0, 0, 1, 1};
const static char* t10_res[] =
  {t10_res_0, t10_res_1};

const static size_t t10_n_output_items = sizeof (t10_res_0);
const static size_t t10_n_code_outputs = sizeof (t10_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t10
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t10_in, (const char**) t10_res,
		    1, (const size_t*) &t10_n_input_items, 
		    (const size_t*) &t10_n_output_items, 5,
		    t10_n_code_inputs, t10_n_code_outputs,
		    (const int*) t10_code_generator,
		    0, true, 0, t10_term_state);
}

// TEST 11
//
// checking for:
// start state is not 0, no feedback, terminating to 0 state

const static int t11_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t11_start_state = 1;

const static char t11_in_0[] =
  {0, 1, 0, 0, 0, 1, 0};
const static char t11_in_1[] =
  {0, 0, 1, 0, 0, 1, 0};
const static char t11_in_2[] =
  {0, 1, 1, 1, 0, 1, 1};
const static char* t11_in[] =
  {t11_in_0, t11_in_1, t11_in_2};

const static size_t t11_n_input_items = sizeof (t11_in_0);
const static size_t t11_n_code_inputs = sizeof (t11_in) / sizeof (char*);

const static char t11_res_0[] =
  {1, 0, 0, 0, 1, 0, 0, 1, 0};
const static char t11_res_1[] =
  {1, 1, 0, 0, 0, 1, 0, 0, 0};
const static char* t11_res[] =
  {t11_res_0, t11_res_1};

const static size_t t11_n_output_items = sizeof (t11_res_0);
const static size_t t11_n_code_outputs = sizeof (t11_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t11
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t11_in, (const char**) t11_res,
		    1, (const size_t*) &t11_n_input_items, 
		    (const size_t*) &t11_n_output_items, 5,
		    t11_n_code_inputs, t11_n_code_outputs,
		    (const int*) t11_code_generator,
		    0, true, t11_start_state);
}

// TEST 12
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state

const static int t12_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t12_start_state = 2;
const static size_t t12_term_state = 2;

const static char t12_in_0[] =
  {0, 1, 1, 1, 0, 1, 0};
const static char t12_in_1[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t12_in_2[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char* t12_in[] =
  {t12_in_0, t12_in_1, t12_in_2};

const static size_t t12_n_input_items = sizeof (t12_in_0);
const static size_t t12_n_code_inputs = sizeof (t12_in) / sizeof (char*);

const static char t12_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1};
const static char t12_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1};
const static char* t12_res[] =
  {t12_res_0, t12_res_1};

const static size_t t12_n_output_items = sizeof (t12_res_0);
const static size_t t12_n_code_outputs = sizeof (t12_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t12
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t12_in, (const char**) t12_res,
		    1, (const size_t*) &t12_n_input_items,
		    (const size_t*) &t12_n_output_items, 5, t12_n_code_inputs,
		    t12_n_code_outputs, (const int*) t12_code_generator,
		    0, true, t12_start_state, t12_term_state);
}

// TEST 13
//
// checking for:
// start state is not 0, any feedback, termination to 0 state

const static int t13_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t13_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t13_start_state = 2;
const static size_t t13_term_state = 0;

const static char t13_in_0[] =
  {0, 1, 0, 0, 0, 1, 0};
const static char t13_in_1[] =
  {0, 0, 1, 1, 0, 1, 0};
const static char t13_in_2[] =
  {0, 0, 0, 0, 1, 0, 0};
const static char* t13_in[] =
  {t13_in_0, t13_in_1, t13_in_2};

const static size_t t13_n_input_items = sizeof (t13_in_0);
const static size_t t13_n_code_inputs = sizeof (t13_in) / sizeof (char*);

const static char t13_res_0[] =
  {0, 0, 1, 0, 0, 1, 0, 1, 1};
const static char t13_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 0, 1};
const static char* t13_res[] =
  {t13_res_0, t13_res_1};

const static size_t t13_n_output_items = sizeof (t13_res_0);
const static size_t t13_n_code_outputs = sizeof (t13_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t13
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t13_in, (const char**) t13_res,
		    1, (const size_t*) &t13_n_input_items,
		    (const size_t*) &t13_n_output_items, 5,
		    t13_n_code_inputs, t13_n_code_outputs,
		    (const int*) t13_code_generator,
		    (const int*) t13_code_feedback,
		    true, t13_start_state, t13_term_state);
}

// TEST 14
//
// checking for:
// start state is not 0, any feedback, termination to non-zero state

const static int t14_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t14_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t14_start_state = 1;
const static size_t t14_term_state = 2;

const static char t14_in_0[] =
  {0, 1, 0, 1, 0, 1, 0};
const static char t14_in_1[] =
  {0, 0, 0, 0, 1, 1, 0};
const static char t14_in_2[] =
  {0, 1, 0, 0, 0, 1, 1};
const static char* t14_in[] =
  {t14_in_0, t14_in_1, t14_in_2};

const static size_t t14_n_input_items = sizeof (t14_in_0);
const static size_t t14_n_code_inputs = sizeof (t14_in) / sizeof (char*);

const static char t14_res_0[] =
  {1, 1, 1, 1, 1, 0, 1, 1, 1};
const static char t14_res_1[] =
  {1, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t14_res[] =
  {t14_res_0, t14_res_1};

const static size_t t14_n_output_items = sizeof (t14_res_0);
const static size_t t14_n_code_outputs = sizeof (t14_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t14
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t14_in, (const char**) t14_res,
		    1, (const size_t*) &t14_n_input_items,
		    (const size_t*) &t14_n_output_items, 5,
		    t14_n_code_inputs, t14_n_code_outputs,
		    (const int*) t14_code_generator,
		    (const int*) t14_code_feedback,
		    true, t14_start_state, t14_term_state);
}

// TEST 15
//
// checking for:
// no feedback, block coding but no termination

const static int    t15_code_generator[] = {1, 0, 3, 0, 1, 6};

const static char t15_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t15_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t15_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t15_in[] =
  {t15_in_0, t15_in_1, t15_in_2};

const static size_t t15_n_input_items = sizeof (t15_in_0);
const static size_t t15_n_code_inputs = sizeof (t15_in) / sizeof (char*);

const static char t15_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0};
const static char t15_res_1[] =
  {0, 1, 1, 1, 0, 0, 1, 1};
const static char* t15_res[] =
  {t15_res_0, t15_res_1};

const static size_t t15_n_output_items = sizeof (t15_res_0);
const static size_t t15_n_code_outputs = sizeof (t15_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t15
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t15_in, (const char**) t15_res,
		    1, (const size_t*) &t15_n_input_items, 
		    (const size_t*) &t15_n_output_items, 5,
		    t15_n_code_inputs, t15_n_code_outputs,
		    (const int*) t15_code_generator, 0, false);
}

// TEST 16
//
// checking for:
// start state is 0, same feedback, block but no termination

const static int t16_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t16_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t16_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t16_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t16_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t16_in[] =
  {t16_in_0, t16_in_1, t16_in_2};

const static size_t t16_n_input_items = sizeof (t16_in_0);
const static size_t t16_n_code_inputs = sizeof (t16_in) / sizeof (char*);

const static char t16_res_0[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char t16_res_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char* t16_res[] =
  {t16_res_0, t16_res_1};

const static size_t t16_n_output_items = sizeof (t16_res_0);
const static size_t t16_n_code_outputs = sizeof (t16_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t16
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t16_in, (const char**) t16_res,
		    1, (const size_t*) &t16_n_input_items, 
		    (const size_t*) &t16_n_output_items, 5, t16_n_code_inputs,
		    t16_n_code_outputs, (const int*) t16_code_generator,
		    (const int*) t16_code_feedback, false);
}

// TEST 17
//
// checking for:
// state state is 0, different feedbacks, block but no termination

const static int t17_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t17_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t17_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t17_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t17_in[] =
  {t17_in_0, t17_in_1};

const static size_t t17_n_input_items = sizeof (t17_in_0);
const static size_t t17_n_code_inputs = sizeof (t17_in) / sizeof (char*);

const static char t17_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 1, 1};
const static char t17_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t17_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 1, 0};
const static char* t17_res[] =
  {t17_res_0, t17_res_1, t17_res_2};

const static size_t t17_n_output_items = sizeof (t17_res_0);
const static size_t t17_n_code_outputs = sizeof (t17_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t17
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t17_in, (const char**) t17_res,
		    1, (const size_t*) &t17_n_input_items, 
		    (const size_t*) &t17_n_output_items, 6, t17_n_code_inputs,
		    t17_n_code_outputs, (const int*) t17_code_generator,
		    (const int*) t17_code_feedback, false);
}

// TEST 18
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state
// multi-encode, stop output before term bits

const static int t18_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t18_start_state = 2;
const static size_t t18_term_state = 2;

const static char t18_in_0[] =
  {0, 1, 1, 1, 0, 1, 0};
const static char t18_in_1[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t18_in_2[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char* t18_in[] =
  {t18_in_0, t18_in_1, t18_in_2};

const static size_t t18_n_input_items[] = {3, 2, 2};
const static size_t t18_n_io_items = (sizeof (t18_n_input_items) /
				      sizeof (size_t));
const static size_t t18_n_code_inputs = sizeof (t18_in) / sizeof (char*);

const static char t18_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1};
const static char t18_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1};
const static char* t18_res[] =
  {t18_res_0, t18_res_1};

const static size_t t18_n_output_items[] = {3, 2, 4};
const static size_t t18_n_code_outputs = sizeof (t18_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t18
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t18_in, (const char**) t18_res,
		    t18_n_io_items, (const size_t*) t18_n_input_items,
		    (const size_t*) t18_n_output_items, 5, t18_n_code_inputs,
		    t18_n_code_outputs, (const int*) t18_code_generator,
		    0, true, t18_start_state, t18_term_state);
}

// TEST 19
//
// checking for:
// start state is not 0, any feedback, termination to non-zero state
// multi-encode, stop output in the middle of term bits

const static int t19_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t19_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t19_start_state = 1;
const static size_t t19_term_state = 2;

const static char t19_in_0[] =
  {0, 1, 0, 1, 0, 1, 0};
const static char t19_in_1[] =
  {0, 0, 0, 0, 1, 1, 0};
const static char t19_in_2[] =
  {0, 1, 0, 0, 0, 1, 1};
const static char* t19_in[] =
  {t19_in_0, t19_in_1, t19_in_2};

const static size_t t19_n_input_items[] = {2, 2, 1, 1, 1};
const static size_t t19_n_io_items = (sizeof (t19_n_input_items) /
				      sizeof (size_t));
const static size_t t19_n_code_inputs = sizeof (t19_in) / sizeof (char*);

const static char t19_res_0[] =
  {1, 1, 1, 1, 1, 0, 1, 1, 1};
const static char t19_res_1[] =
  {1, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t19_res[] =
  {t19_res_0, t19_res_1};

const static size_t t19_n_output_items[] = {2, 2, 2, 2, 1};
const static size_t t19_n_code_outputs = sizeof (t19_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t19
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t19_in, (const char**) t19_res,
		    1, (const size_t*) &t19_n_input_items,
		    (const size_t*) &t19_n_output_items, 5,
		    t19_n_code_inputs, t19_n_code_outputs,
		    (const int*) t19_code_generator,
		    (const int*) t19_code_feedback,
		    true, t19_start_state, t19_term_state);
}

// TEST 20
//
// checking for:
// no feedback, block coding but no termination
// multi-encode

const static int    t20_code_generator[] = {1, 0, 3, 0, 1, 6};

const static char t20_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t20_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t20_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t20_in[] =
  {t20_in_0, t20_in_1, t20_in_2};

const static size_t t20_n_input_items[] = {2, 2, 2, 2};
const static size_t t20_n_n_input_items = (sizeof (t20_n_input_items) /
					   sizeof (size_t));
const static size_t t20_n_code_inputs = sizeof (t20_in) / sizeof (char*);

const static char t20_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0};
const static char t20_res_1[] =
  {0, 1, 1, 1, 0, 0, 1, 1};
const static char* t20_res[] =
  {t20_res_0, t20_res_1};

const static size_t t20_n_output_items[] = {2, 2, 2, 2};
const static size_t t20_n_code_outputs = sizeof (t20_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t20
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t20_in, (const char**) t20_res,
		    t20_n_n_input_items, (const size_t*) t20_n_input_items, 
		    (const size_t*) t20_n_output_items, 5,
		    t20_n_code_inputs, t20_n_code_outputs,
		    (const int*) t20_code_generator, 0, false);
}

// TEST 21
//
// checking for:
// start state is 0, same feedback, block but no termination
// multi-encode

const static int t21_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t21_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t21_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t21_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t21_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t21_in[] =
  {t21_in_0, t21_in_1, t21_in_2};

const static size_t t21_n_input_items[] = {5, 1, 1};
const static size_t t21_n_n_input_items = (sizeof (t21_n_input_items) /
					   sizeof (size_t));
const static size_t t21_n_code_inputs = sizeof (t21_in) / sizeof (char*);

const static char t21_res_0[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char t21_res_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char* t21_res[] =
  {t21_res_0, t21_res_1};

const static size_t t21_n_output_items[] = {5, 1, 1};
const static size_t t21_n_code_outputs = sizeof (t21_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t21
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t21_in, (const char**) t21_res,
		    t21_n_n_input_items, (const size_t*) t21_n_input_items, 
		    (const size_t*) t21_n_output_items, 5, t21_n_code_inputs,
		    t21_n_code_outputs, (const int*) t21_code_generator,
		    (const int*) t21_code_feedback, false);
}

// TEST 22
//
// checking for:
// state state is 0, different feedbacks, block but no termination
// multi-encode

const static int t22_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t22_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t22_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t22_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t22_in[] =
  {t22_in_0, t22_in_1};

const static size_t t22_n_input_items[] = {5, 4};
const static size_t t22_n_n_input_items = (sizeof (t22_n_input_items) /
					   sizeof (size_t));
const static size_t t22_n_code_inputs = sizeof (t22_in) / sizeof (char*);

const static char t22_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 1, 1};
const static char t22_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t22_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 1, 0};
const static char* t22_res[] =
  {t22_res_0, t22_res_1, t22_res_2};

const static size_t t22_n_output_items[] = {5, 4};
const static size_t t22_n_code_outputs = sizeof (t22_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t22
()
{
  do_encoder_check (g_do_encode_in_or_out_0_22,
		    (const char**) t22_in, (const char**) t22_res,
		    t22_n_n_input_items, (const size_t*) t22_n_input_items, 
		    (const size_t*) t22_n_output_items, 6, t22_n_code_inputs,
		    t22_n_code_outputs, (const int*) t22_code_generator,
		    (const int*) t22_code_feedback, false);
}

// TESTS 23 through 45 use do_encode_in_or_out as true

static const bool g_do_encode_in_or_out_23_45 = true;

// TEST 23
//
// checking for:
// SIAO realization (implicitely)
// start state is 0, no feedback, no termination

const static int t23_code_generator[] = {1, 0, 5, 0, 1, 6};
const static size_t t23_encode_soai = 0;

const static char t23_in_0[] =
  {0, 1, 0, 0, 1, 0, 1, 0, 0, 0};
const static char t23_in_1[] =
  {0, 1, 0, 0, 0, 1, 1, 0, 0, 0};
const static char t23_in_2[] =
  {0, 0, 1, 1, 1, 1, 1, 0, 0, 0};
const static char* t23_in[] =
  {t23_in_0, t23_in_1, t23_in_2};

const static size_t t23_n_input_items = sizeof (t23_in_0);
const static size_t t23_n_code_inputs = sizeof (t23_in) / sizeof (char*);

const static char t23_res_0[] =
  {0, 1, 1, 1, 1, 0, 1, 1, 1, 0};
const static char t23_res_1[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 1, 0};
const static char* t23_res[] =
  {t23_res_0, t23_res_1};

const static size_t t23_n_output_items = sizeof (t23_res_0);
const static size_t t23_n_code_outputs = sizeof (t23_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t23
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t23_in, (const char**) t23_res,
		    1, (const size_t*) &t23_n_input_items, 
		    (const size_t*) &t23_n_output_items, 100,
		    t23_n_code_inputs, t23_n_code_outputs,
		    (const int*) t23_code_generator,
		    0, true, 0, 0, t23_encode_soai);
}

// TEST 24
//
// checking for SIAO realization (implicitely)
// start state is 0, no feedback, no termination

const static int t24_code_generator[] = {1, 0, 0, 1, 5, 6};
const static size_t t24_encode_soai = 1;

const static char t24_in_0[] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t24_in_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char* t24_in[] =
  {t24_in_0, t24_in_1};

const static size_t t24_n_input_items = sizeof (t24_in_0);
const static size_t t24_n_code_inputs = sizeof (t24_in) / sizeof (char*);

const static char t24_res_0[] =
  {0, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t24_res_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0};
const static char t24_res_2[] =
  {0, 1, 1, 0, 1, 1, 1, 1, 0};
const static char* t24_res[] =
  {t24_res_0, t24_res_1, t24_res_2};

const static size_t t24_n_output_items = sizeof (t24_res_0);
const static size_t t24_n_code_outputs = sizeof (t24_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t24
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t24_in, (const char**) t24_res,
		    1, (const size_t*) &t24_n_input_items, 
		    (const size_t*) &t24_n_output_items, 100,
		    t24_n_code_inputs, t24_n_code_outputs,
		    (const int*) t24_code_generator,
		    0, true, 0, 0, t24_encode_soai);
}

// TEST 25
//
// checking for SOAI realization (implicitely)
// start state is 0, same feedback, no termination

const static int t25_code_generator[] = {1, 0, 0, 1, 5, 6};
const static size_t t25_code_feedback[] = {0, 0, 0, 0, 7, 7};
const static int t25_encode_soai = 1;

const static char t25_in_0[] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t25_in_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char* t25_in[] =
  {t25_in_0, t25_in_1};

const static size_t t25_n_input_items = sizeof (t25_in_0);
const static size_t t25_n_code_inputs = sizeof (t25_in) / sizeof (char*);

const static char t25_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0};
const static char t25_res_1[] =
  {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
const static char t25_res_2[] =
  {0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0};
const static char* t25_res[] =
  {t25_res_0, t25_res_1, t25_res_2};

const static size_t t25_n_output_items = sizeof (t25_res_0);
const static size_t t25_n_code_outputs = sizeof (t25_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t25
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t25_in, (const char**) t25_res,
		    1, (const size_t*) &t25_n_input_items, 
		    (const size_t*) &t25_n_output_items, 100,
		    t25_n_code_inputs, t25_n_code_outputs,
		    (const int*) t25_code_generator,
		    (const int*) t25_code_feedback,
		    true, 0, 0, t25_encode_soai);
}

// TEST 26
//
// checking for SIAO realization (implicitely)
// start state is 0, same feedback, no termination

const static int t26_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t26_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static int t26_encode_soai = 0;

const static char t26_in_0[] =
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0};
const static char t26_in_1[] =
  {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static char t26_in_2[] =
  {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0};
const static char* t26_in[] =
  {t26_in_0, t26_in_1, t26_in_2};

const static size_t t26_n_input_items = sizeof (t26_in_0);
const static size_t t26_n_code_inputs = sizeof (t26_in) / sizeof (char*);

const static char t26_res_0[] =
  {0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
const static char t26_res_1[] =
  {0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0};
const static char* t26_res[] =
  {t26_res_0, t26_res_1};

const static size_t t26_n_output_items = sizeof (t26_res_0);
const static size_t t26_n_code_outputs = sizeof (t26_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t26
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t26_in, (const char**) t26_res,
		    1, (const size_t*) &t26_n_input_items, 
		    (const size_t*) &t26_n_output_items, 100,
		    t26_n_code_inputs, t26_n_code_outputs,
		    (const int*) t26_code_generator,
		    (const int*) t26_code_feedback,
		    true, 0, 0, t26_encode_soai);
}

// TEST 27
//
// checking for SIAO realization (implicitely),
// start state is 0, different feedbacks, no termination

const static int t27_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t27_code_feedback[] = {0, 7, 0, 5, 0, 5};
const static int t27_encode_soai = 0;

const static char t27_in_0[] =
  {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0};
const static char t27_in_1[] =
  {0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t27_in[] =
  {t27_in_0, t27_in_1};

const static size_t t27_n_input_items = sizeof (t27_in_0);
const static size_t t27_n_code_inputs = sizeof (t27_in) / sizeof (char*);

const static char t27_res_0[] =
  {0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0};
const static char t27_res_1[] =
  {0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0};
const static char t27_res_2[] =
  {0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0};
const static char* t27_res[] =
  {t27_res_0, t27_res_1, t27_res_2};

const static size_t t27_n_output_items = sizeof (t27_res_0);
const static size_t t27_n_code_outputs = sizeof (t27_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t27
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t27_in, (const char**) t27_res,
		    1, (const size_t*) &t27_n_input_items, 
		    (const size_t*) &t27_n_output_items, 100,
		    t27_n_code_inputs, t27_n_code_outputs,
		    (const int*) t27_code_generator,
		    (const int*) t27_code_feedback,
		    true, 0, 0, t27_encode_soai);
}

// TEST 28
//
// checking for SOAI realization (implicitely),
// with different feedbacks, no termination

const static int t28_code_generator[] = {1, 0, 1, 4, 3, 6};
const static int t28_code_feedback[] = {0, 0, 0, 5, 7, 7};
const static int t28_encode_soai = 1;

const static char t28_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 0, 1};
const static char t28_in_1[] =
  {0, 0, 1, 0, 1, 1, 0, 0, 1};
const static char t28_in_2[] =
  {0, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t28_in[] =
  {t28_in_0, t28_in_1, t28_in_2};

const static size_t t28_n_input_items = sizeof (t28_in_0);
const static size_t t28_n_code_inputs = sizeof (t28_in) / sizeof (char*);

const static char t28_res_0[] =
  {0, 1, 0, 1, 0, 1, 0, 0, 0};
const static char t28_res_1[] =
  {0, 0, 1, 1, 1, 1, 0, 0, 1};
const static char* t28_res[] =
  {t28_res_0, t28_res_1};

const static size_t t28_n_output_items = sizeof (t28_res_0);
const static size_t t28_n_code_outputs = sizeof (t28_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t28
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t28_in, (const char**) t28_res,
		    1, (const size_t*) &t28_n_input_items,
		    (const size_t*) &t28_n_output_items, 100,
		    t28_n_code_inputs, t28_n_code_outputs,
		    (const int*) t28_code_generator,
		    (const int*) t28_code_feedback, true,
		    0, 0, t28_encode_soai);
}

// TEST 29
//
// checking for:
// start state is 0, no feedback, termination to 0 state

const static int t29_code_generator[] = {1, 0, 5, 0, 1, 6};

const static char t29_in_0[] =
  {0, 1, 0, 0, 1, 0};
const static char t29_in_1[] =
  {0, 1, 0, 0, 0, 0};
const static char t29_in_2[] =
  {0, 0, 1, 1, 1, 0};
const static char* t29_in[] =
  {t29_in_0, t29_in_1, t29_in_2};

const static size_t t29_n_input_items = sizeof (t29_in_0);
const static size_t t29_n_code_inputs = sizeof (t29_in) / sizeof (char*);

const static char t29_res_0[] =
  {0, 1, 1, 1, 1, 1, 1, 0};
const static char t29_res_1[] =
  {0, 1, 0, 1, 0, 0, 1, 0};
const static char* t29_res[] =
  {t29_res_0, t29_res_1};

const static size_t t29_n_output_items = sizeof (t29_res_0);
const static size_t t29_n_code_outputs = sizeof (t29_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t29
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t29_in, (const char**) t29_res,
		    1, (const size_t*) &t29_n_input_items, 
		    (const size_t*) &t29_n_output_items, 5, t29_n_code_inputs,
		    t29_n_code_outputs, (const int*) t29_code_generator);
}

// TEST 30
//
// checking for:
// start state is 0, same feedback, termination to 0 state
// # of termination bits = 2

const static int t30_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t30_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t30_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t30_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t30_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t30_in[] =
  {t30_in_0, t30_in_1, t30_in_2};

const static size_t t30_n_input_items = sizeof (t30_in_0);
const static size_t t30_n_code_inputs = sizeof (t30_in) / sizeof (char*);

const static char t30_res_0[] =
  {0, 1, 1, 0, 1, 0, 1, 0, 1};
const static char t30_res_1[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 0};
const static char* t30_res[] =
  {t30_res_0, t30_res_1};

const static size_t t30_n_output_items = sizeof (t30_res_0);
const static size_t t30_n_code_outputs = sizeof (t30_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t30
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t30_in, (const char**) t30_res,
		    1, (const size_t*) &t30_n_input_items, 
		    (const size_t*) &t30_n_output_items, 5, t30_n_code_inputs,
		    t30_n_code_outputs, (const int*) t30_code_generator,
		    (const int*) t30_code_feedback);
}

// TEST 31
//
// checking for:
// state state is 0, different feedbacks, termination to 0 state
// # of term bits will be 4

const static int t31_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t31_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t31_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t31_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t31_in[] =
  {t31_in_0, t31_in_1};

const static size_t t31_n_input_items = sizeof (t31_in_0);
const static size_t t31_n_code_inputs = sizeof (t31_in) / sizeof (char*);

const static char t31_res_0[] =
  {0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1};
const static char t31_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t31_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0};
const static char* t31_res[] =
  {t31_res_0, t31_res_1, t31_res_2};

const static size_t t31_n_output_items = sizeof (t31_res_0);
const static size_t t31_n_code_outputs = sizeof (t31_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t31
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t31_in, (const char**) t31_res,
		    1, (const size_t*) &t31_n_input_items, 
		    (const size_t*) &t31_n_output_items, 6, t31_n_code_inputs,
		    t31_n_code_outputs, (const int*) t31_code_generator,
		    (const int*) t31_code_feedback);
}

// TEST 32
//
// checking for:
// start state is 0, different feedbacks, termination to non-0 state
// # of term bits will be 2

const static int t32_code_generator[] = {1, 0, 1, 4, 3, 6};
const static int t32_code_feedback[] = {0, 0, 0, 5, 7, 7};
const static size_t t32_term_state = 4;

const static char t32_in_0[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t32_in_1[] =
  {0, 0, 1, 0, 1, 0, 1};
const static char t32_in_2[] =
  {0, 0, 0, 1, 0, 0, 1};
const static char* t32_in[] =
  {t32_in_0, t32_in_1, t32_in_2};

const static size_t t32_n_input_items = sizeof (t32_in_0);
const static size_t t32_n_code_inputs = sizeof (t32_in) / sizeof (char*);

const static char t32_res_0[] =
  {0, 1, 0, 1, 0, 1, 1, 0, 0};
const static char t32_res_1[] =
  {0, 0, 1, 1, 1, 1, 1, 0, 1};
const static char* t32_res[] =
  {t32_res_0, t32_res_1};

const static size_t t32_n_output_items = sizeof (t32_res_0);
const static size_t t32_n_code_outputs = sizeof (t32_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t32
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t32_in, (const char**) t32_res,
		    1, (const size_t*) &t32_n_input_items,
		    (const size_t*) &t32_n_output_items, 5, t32_n_code_inputs,
		    t32_n_code_outputs, (const int*) t32_code_generator,
		    (const int*) t32_code_feedback, true, 0, t32_term_state);
}

// TEST 33
//
// checking for termination to non-0 state, no feedback

const static int t33_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t33_term_state = 2;

const static char t33_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t33_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t33_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t33_in[] =
  {t33_in_0, t33_in_1, t33_in_2};

const static size_t t33_n_input_items = sizeof (t33_in_0);
const static size_t t33_n_code_inputs = sizeof (t33_in) / sizeof (char*);

const static char t33_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
const static char t33_res_1[] =
  {0, 1, 1, 1, 0, 0, 0, 0, 1, 1};
const static char* t33_res[] =
  {t33_res_0, t33_res_1};

const static size_t t33_n_output_items = sizeof (t33_res_0);
const static size_t t33_n_code_outputs = sizeof (t33_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t33
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t33_in, (const char**) t33_res,
		    1, (const size_t*) &t33_n_input_items, 
		    (const size_t*) &t33_n_output_items, 5,
		    t33_n_code_inputs, t33_n_code_outputs,
		    (const int*) t33_code_generator,
		    0, true, 0, t33_term_state);
}

// TEST 34
//
// checking for:
// start state is not 0, no feedback, terminating to 0 state

const static int t34_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t34_start_state = 1;

const static char t34_in_0[] =
  {0, 1, 0, 0, 0, 1, 0};
const static char t34_in_1[] =
  {0, 0, 1, 0, 0, 1, 0};
const static char t34_in_2[] =
  {0, 1, 1, 1, 0, 1, 1};
const static char* t34_in[] =
  {t34_in_0, t34_in_1, t34_in_2};

const static size_t t34_n_input_items = sizeof (t34_in_0);
const static size_t t34_n_code_inputs = sizeof (t34_in) / sizeof (char*);

const static char t34_res_0[] =
  {1, 0, 0, 0, 1, 0, 0, 1, 0};
const static char t34_res_1[] =
  {1, 1, 0, 0, 0, 1, 0, 0, 0};
const static char* t34_res[] =
  {t34_res_0, t34_res_1};

const static size_t t34_n_output_items = sizeof (t34_res_0);
const static size_t t34_n_code_outputs = sizeof (t34_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t34
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t34_in, (const char**) t34_res,
		    1, (const size_t*) &t34_n_input_items, 
		    (const size_t*) &t34_n_output_items, 5,
		    t34_n_code_inputs, t34_n_code_outputs,
		    (const int*) t34_code_generator,
		    0, true, t34_start_state);
}

// TEST 35
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state

const static int t35_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t35_start_state = 2;
const static size_t t35_term_state = 2;

const static char t35_in_0[] =
  {0, 1, 1, 1, 0, 1, 0};
const static char t35_in_1[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t35_in_2[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char* t35_in[] =
  {t35_in_0, t35_in_1, t35_in_2};

const static size_t t35_n_input_items = sizeof (t35_in_0);
const static size_t t35_n_code_inputs = sizeof (t35_in) / sizeof (char*);

const static char t35_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1};
const static char t35_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1};
const static char* t35_res[] =
  {t35_res_0, t35_res_1};

const static size_t t35_n_output_items = sizeof (t35_res_0);
const static size_t t35_n_code_outputs = sizeof (t35_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t35
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t35_in, (const char**) t35_res,
		    1, (const size_t*) &t35_n_input_items,
		    (const size_t*) &t35_n_output_items, 5, t35_n_code_inputs,
		    t35_n_code_outputs, (const int*) t35_code_generator,
		    0, true, t35_start_state, t35_term_state);
}

// TEST 36
//
// checking for:
// start state is not 0, any feedback, termination to 0 state

const static int t36_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t36_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t36_start_state = 2;
const static size_t t36_term_state = 0;

const static char t36_in_0[] =
  {0, 1, 0, 0, 0, 1, 0};
const static char t36_in_1[] =
  {0, 0, 1, 1, 0, 1, 0};
const static char t36_in_2[] =
  {0, 0, 0, 0, 1, 0, 0};
const static char* t36_in[] =
  {t36_in_0, t36_in_1, t36_in_2};

const static size_t t36_n_input_items = sizeof (t36_in_0);
const static size_t t36_n_code_inputs = sizeof (t36_in) / sizeof (char*);

const static char t36_res_0[] =
  {0, 0, 1, 0, 0, 1, 0, 1, 1};
const static char t36_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 0, 1};
const static char* t36_res[] =
  {t36_res_0, t36_res_1};

const static size_t t36_n_output_items = sizeof (t36_res_0);
const static size_t t36_n_code_outputs = sizeof (t36_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t36
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t36_in, (const char**) t36_res,
		    1, (const size_t*) &t36_n_input_items,
		    (const size_t*) &t36_n_output_items, 5,
		    t36_n_code_inputs, t36_n_code_outputs,
		    (const int*) t36_code_generator,
		    (const int*) t36_code_feedback,
		    true, t36_start_state, t36_term_state);
}

// TEST 37
//
// checking for:
// start state is not 0, any feedback, termination to non-zero state

const static int t37_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t37_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t37_start_state = 1;
const static size_t t37_term_state = 2;

const static char t37_in_0[] =
  {0, 1, 0, 1, 0, 1, 0};
const static char t37_in_1[] =
  {0, 0, 0, 0, 1, 1, 0};
const static char t37_in_2[] =
  {0, 1, 0, 0, 0, 1, 1};
const static char* t37_in[] =
  {t37_in_0, t37_in_1, t37_in_2};

const static size_t t37_n_input_items = sizeof (t37_in_0);
const static size_t t37_n_code_inputs = sizeof (t37_in) / sizeof (char*);

const static char t37_res_0[] =
  {1, 1, 1, 1, 1, 0, 1, 1, 1};
const static char t37_res_1[] =
  {1, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t37_res[] =
  {t37_res_0, t37_res_1};

const static size_t t37_n_output_items = sizeof (t37_res_0);
const static size_t t37_n_code_outputs = sizeof (t37_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t37
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t37_in, (const char**) t37_res,
		    1, (const size_t*) &t37_n_input_items,
		    (const size_t*) &t37_n_output_items, 5,
		    t37_n_code_inputs, t37_n_code_outputs,
		    (const int*) t37_code_generator,
		    (const int*) t37_code_feedback,
		    true, t37_start_state, t37_term_state);
}

// TEST 38
//
// checking for:
// no feedback, block coding but no termination

const static int    t38_code_generator[] = {1, 0, 3, 0, 1, 6};

const static char t38_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t38_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t38_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t38_in[] =
  {t38_in_0, t38_in_1, t38_in_2};

const static size_t t38_n_input_items = sizeof (t38_in_0);
const static size_t t38_n_code_inputs = sizeof (t38_in) / sizeof (char*);

const static char t38_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0};
const static char t38_res_1[] =
  {0, 1, 1, 1, 0, 0, 1, 1};
const static char* t38_res[] =
  {t38_res_0, t38_res_1};

const static size_t t38_n_output_items = sizeof (t38_res_0);
const static size_t t38_n_code_outputs = sizeof (t38_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t38
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t38_in, (const char**) t38_res,
		    1, (const size_t*) &t38_n_input_items, 
		    (const size_t*) &t38_n_output_items, 5,
		    t38_n_code_inputs, t38_n_code_outputs,
		    (const int*) t38_code_generator, 0, false);
}

// TEST 39
//
// checking for:
// start state is 0, same feedback, block but no termination

const static int t39_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t39_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t39_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t39_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t39_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t39_in[] =
  {t39_in_0, t39_in_1, t39_in_2};

const static size_t t39_n_input_items = sizeof (t39_in_0);
const static size_t t39_n_code_inputs = sizeof (t39_in) / sizeof (char*);

const static char t39_res_0[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char t39_res_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char* t39_res[] =
  {t39_res_0, t39_res_1};

const static size_t t39_n_output_items = sizeof (t39_res_0);
const static size_t t39_n_code_outputs = sizeof (t39_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t39
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t39_in, (const char**) t39_res,
		    1, (const size_t*) &t39_n_input_items, 
		    (const size_t*) &t39_n_output_items, 5, t39_n_code_inputs,
		    t39_n_code_outputs, (const int*) t39_code_generator,
		    (const int*) t39_code_feedback, false);
}

// TEST 40
//
// checking for:
// state state is 0, different feedbacks, block but no termination

const static int t40_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t40_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t40_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t40_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t40_in[] =
  {t40_in_0, t40_in_1};

const static size_t t40_n_input_items = sizeof (t40_in_0);
const static size_t t40_n_code_inputs = sizeof (t40_in) / sizeof (char*);

const static char t40_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 1, 1};
const static char t40_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t40_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 1, 0};
const static char* t40_res[] =
  {t40_res_0, t40_res_1, t40_res_2};

const static size_t t40_n_output_items = sizeof (t40_res_0);
const static size_t t40_n_code_outputs = sizeof (t40_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t40
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t40_in, (const char**) t40_res,
		    1, (const size_t*) &t40_n_input_items, 
		    (const size_t*) &t40_n_output_items, 6, t40_n_code_inputs,
		    t40_n_code_outputs, (const int*) t40_code_generator,
		    (const int*) t40_code_feedback, false);
}

// TEST 41
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state
// multi-encode, forced to retrieve all term outputs

const static int t41_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t41_start_state = 2;
const static size_t t41_term_state = 2;

const static char t41_in_0[] =
  {0, 1, 1, 1, 0, 1, 0};
const static char t41_in_1[] =
  {0, 1, 0, 0, 0, 0, 1};
const static char t41_in_2[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char* t41_in[] =
  {t41_in_0, t41_in_1, t41_in_2};

const static size_t t41_n_input_items[] = {3, 2, 2};
const static size_t t41_n_io_items = (sizeof (t41_n_input_items) /
				      sizeof (size_t));
const static size_t t41_n_code_inputs = sizeof (t41_in) / sizeof (char*);

const static char t41_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1};
const static char t41_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1};
const static char* t41_res[] =
  {t41_res_0, t41_res_1};

const static size_t t41_n_output_items[] = {3, 4, 2};
const static size_t t41_n_code_outputs = sizeof (t41_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t41
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t41_in, (const char**) t41_res,
		    t41_n_io_items, (const size_t*) t41_n_input_items,
		    (const size_t*) t41_n_output_items, 5, t41_n_code_inputs,
		    t41_n_code_outputs, (const int*) t41_code_generator,
		    0, true, t41_start_state, t41_term_state);
}

// TEST 42
//
// checking for:
// start state is not 0, any feedback, termination to non-zero state
// multi-encode

const static int t42_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t42_code_feedback[] = {0, 0, 7, 0, 0, 7};
const static size_t t42_start_state = 1;
const static size_t t42_term_state = 2;

const static char t42_in_0[] =
  {0, 1, 0, 1, 0, 1, 0};
const static char t42_in_1[] =
  {0, 0, 0, 0, 1, 1, 0};
const static char t42_in_2[] =
  {0, 1, 0, 0, 0, 1, 1};
const static char* t42_in[] =
  {t42_in_0, t42_in_1, t42_in_2};

const static size_t t42_n_input_items[] = {2, 2, 1, 1, 1};
const static size_t t42_n_io_items = (sizeof (t42_n_input_items) /
				      sizeof (size_t));
const static size_t t42_n_code_inputs = sizeof (t42_in) / sizeof (char*);

const static char t42_res_0[] =
  {1, 1, 1, 1, 1, 0, 1, 1, 1};
const static char t42_res_1[] =
  {1, 0, 0, 1, 0, 0, 0, 0, 1};
const static char* t42_res[] =
  {t42_res_0, t42_res_1};

const static size_t t42_n_output_items[] = {2, 2, 3, 1, 1};
const static size_t t42_n_code_outputs = sizeof (t42_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t42
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t42_in, (const char**) t42_res,
		    1, (const size_t*) &t42_n_input_items,
		    (const size_t*) &t42_n_output_items, 5,
		    t42_n_code_inputs, t42_n_code_outputs,
		    (const int*) t42_code_generator,
		    (const int*) t42_code_feedback,
		    true, t42_start_state, t42_term_state);
}

// TEST 43
//
// checking for:
// no feedback, block coding but no termination
// multi-encode

const static int    t43_code_generator[] = {1, 0, 3, 0, 1, 6};

const static char t43_in_0[] =
  {0, 1, 0, 1, 0, 0, 1, 1};
const static char t43_in_1[] =
  {0, 1, 0, 0, 1, 0, 1, 1};
const static char t43_in_2[] =
  {0, 1, 0, 1, 1, 0, 0, 1};
const static char* t43_in[] =
  {t43_in_0, t43_in_1, t43_in_2};

const static size_t t43_n_input_items[] = {2, 2, 2, 2};
const static size_t t43_n_n_input_items = (sizeof (t43_n_input_items) /
					   sizeof (size_t));
const static size_t t43_n_code_inputs = sizeof (t43_in) / sizeof (char*);

const static char t43_res_0[] =
  {0, 0, 1, 0, 0, 0, 1, 0};
const static char t43_res_1[] =
  {0, 1, 1, 1, 0, 0, 1, 1};
const static char* t43_res[] =
  {t43_res_0, t43_res_1};

const static size_t t43_n_output_items[] = {2, 2, 2, 2};
const static size_t t43_n_code_outputs = sizeof (t43_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t43
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t43_in, (const char**) t43_res,
		    t43_n_n_input_items, (const size_t*) t43_n_input_items, 
		    (const size_t*) t43_n_output_items, 5,
		    t43_n_code_inputs, t43_n_code_outputs,
		    (const int*) t43_code_generator, 0, false);
}

// TEST 44
//
// checking for:
// start state is 0, same feedback, block but no termination
// multi-encode

const static int t44_code_generator[] = {1, 0, 5, 0, 1, 6};
const static int t44_code_feedback[] = {0, 0, 7, 0, 0, 7};

const static char t44_in_0[] =
  {0, 1, 1, 0, 0, 0, 0};
const static char t44_in_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char t44_in_2[] =
  {0, 0, 0, 0, 1, 0, 1};
const static char* t44_in[] =
  {t44_in_0, t44_in_1, t44_in_2};

const static size_t t44_n_input_items[] = {5, 1, 1};
const static size_t t44_n_n_input_items = (sizeof (t44_n_input_items) /
					   sizeof (size_t));
const static size_t t44_n_code_inputs = sizeof (t44_in) / sizeof (char*);

const static char t44_res_0[] =
  {0, 1, 1, 0, 1, 0, 1};
const static char t44_res_1[] =
  {0, 1, 0, 1, 0, 0, 0};
const static char* t44_res[] =
  {t44_res_0, t44_res_1};

const static size_t t44_n_output_items[] = {5, 1, 1};
const static size_t t44_n_code_outputs = sizeof (t44_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t44
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t44_in, (const char**) t44_res,
		    t44_n_n_input_items, (const size_t*) t44_n_input_items, 
		    (const size_t*) t44_n_output_items, 5, t44_n_code_inputs,
		    t44_n_code_outputs, (const int*) t44_code_generator,
		    (const int*) t44_code_feedback, false);
}

// TEST 45
//
// checking for:
// state state is 0, different feedbacks, block but no termination
// multi-encode

const static int t45_code_generator[] = {1, 4, 0, 3, 1, 6};
const static int t45_code_feedback[] = {0, 5, 0, 7, 0, 7};

const static char t45_in_0[] =
  {0, 1, 0, 0, 0, 1, 0, 1, 1};
const static char t45_in_1[] =
  {0, 0, 0, 1, 1, 1, 0, 1, 0};
const static char* t45_in[] =
  {t45_in_0, t45_in_1};

const static size_t t45_n_input_items[] = {5, 4};
const static size_t t45_n_n_input_items = (sizeof (t45_n_input_items) /
					   sizeof (size_t));
const static size_t t45_n_code_inputs = sizeof (t45_in) / sizeof (char*);

const static char t45_res_0[] =
  {0, 1, 0, 0, 0, 0, 0, 1, 1};
const static char t45_res_1[] =
  {0, 0, 0, 1, 1, 0, 0, 1, 0};
const static char t45_res_2[] =
  {0, 1, 0, 0, 1, 0, 0, 1, 0};
const static char* t45_res[] =
  {t45_res_0, t45_res_1, t45_res_2};

const static size_t t45_n_output_items[] = {5, 4};
const static size_t t45_n_code_outputs = sizeof (t45_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t45
()
{
  do_encoder_check (g_do_encode_in_or_out_23_45,
		    (const char**) t45_in, (const char**) t45_res,
		    t45_n_n_input_items, (const size_t*) t45_n_input_items, 
		    (const size_t*) t45_n_output_items, 6, t45_n_code_inputs,
		    t45_n_code_outputs, (const int*) t45_code_generator,
		    (const int*) t45_code_feedback, false);
}

// TEST 46
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state
// multi-encode, input dictates encoding: forced to retrieve all term
// outputs; multiple encoding blocks

const static int t46_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t46_start_state = 2;
const static size_t t46_term_state = 2;

const static char t46_in_0[] =
  {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0};
const static char t46_in_1[] =
  {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0};
const static char t46_in_2[] =
  {0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0};
const static char* t46_in[] =
  {t46_in_0, t46_in_1, t46_in_2};

const static size_t t46_n_input_items[] = {5, 0, 4, 1, 1};
const static size_t t46_n_io_items = (sizeof (t46_n_input_items) /
				      sizeof (size_t));
const static size_t t46_n_code_inputs = sizeof (t46_in) / sizeof (char*);

const static char t46_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t46_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1};
const static char* t46_res[] =
  {t46_res_0, t46_res_1};

const static size_t t46_n_output_items[] = {7, 0, 4, 3, 1};
const static size_t t46_n_code_outputs = sizeof (t46_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t46
()
{
  do_encoder_check (true,
		    (const char**) t46_in, (const char**) t46_res,
		    t46_n_io_items, (const size_t*) t46_n_input_items,
		    (const size_t*) t46_n_output_items, 5, t46_n_code_inputs,
		    t46_n_code_outputs, (const int*) t46_code_generator,
		    0, true, t46_start_state, t46_term_state);
}

// TEST 47
//
// checking for:
// start state is not 0, no feedback, terminating to non-0 state
// multi-encode, output dictates encoding; multiple encoding blocks

const static int t47_code_generator[] = {1, 0, 3, 0, 1, 6};
const static size_t t47_start_state = 2;
const static size_t t47_term_state = 2;

const static char t47_in_0[] =
  {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0};
const static char t47_in_1[] =
  {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0};
const static char t47_in_2[] =
  {0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0};
const static char* t47_in[] =
  {t47_in_0, t47_in_1, t47_in_2};

const static size_t t47_n_input_items[] = {5, 0, 0, 4, 1, 0, 0, 1};
const static size_t t47_n_io_items = (sizeof (t47_n_input_items) /
				      sizeof (size_t));
const static size_t t47_n_code_inputs = sizeof (t47_in) / sizeof (char*);

const static char t47_res_0[] =
  {0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0};
const static char t47_res_1[] =
  {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1};
const static char* t47_res[] =
  {t47_res_0, t47_res_1};

const static size_t t47_n_output_items[] = {6, 0, 1, 4, 1, 1, 1, 1};
const static size_t t47_n_code_outputs = sizeof (t47_res) / sizeof (char*);

void
qa_encoder_convolutional_ic1_ic1::t47
()
{
  do_encoder_check (false,
		    (const char**) t47_in, (const char**) t47_res,
		    t47_n_io_items, (const size_t*) t47_n_input_items,
		    (const size_t*) t47_n_output_items, 5, t47_n_code_inputs,
		    t47_n_code_outputs, (const int*) t47_code_generator,
		    0, true, t47_start_state, t47_term_state);
}

