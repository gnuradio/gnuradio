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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_QA_ENCODER_CONVOLUTIONAL_IC1_IC1_H
#define INCLUDED_QA_ENCODER_CONVOLUTIONAL_IC1_IC1_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <stdexcept>

class qa_encoder_convolutional_ic1_ic1 : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE (qa_encoder_convolutional_ic1_ic1);
  CPPUNIT_TEST (t0);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST (t2);
  CPPUNIT_TEST (t3);
  CPPUNIT_TEST (t4);
  CPPUNIT_TEST (t5);
  CPPUNIT_TEST (t6);
  CPPUNIT_TEST (t7);
  CPPUNIT_TEST (t8);
  CPPUNIT_TEST (t9);
  CPPUNIT_TEST (t10);
  CPPUNIT_TEST (t11);
  CPPUNIT_TEST (t12);
  CPPUNIT_TEST (t13);
  CPPUNIT_TEST (t14);
  CPPUNIT_TEST (t15);
  CPPUNIT_TEST (t16);
  CPPUNIT_TEST (t17);
  CPPUNIT_TEST (t18);
  CPPUNIT_TEST (t19);
  CPPUNIT_TEST (t20);
  CPPUNIT_TEST (t21);
  CPPUNIT_TEST (t22);
  CPPUNIT_TEST (t23);
  CPPUNIT_TEST (t24);
  CPPUNIT_TEST (t25);
  CPPUNIT_TEST (t26);
  CPPUNIT_TEST (t27);
  CPPUNIT_TEST (t28);
  CPPUNIT_TEST (t29);
  CPPUNIT_TEST (t30);
  CPPUNIT_TEST (t31);
  CPPUNIT_TEST (t32);
  CPPUNIT_TEST (t33);
  CPPUNIT_TEST (t34);
  CPPUNIT_TEST (t35);
  CPPUNIT_TEST (t36);
  CPPUNIT_TEST (t37);
  CPPUNIT_TEST (t38);
  CPPUNIT_TEST (t39);
  CPPUNIT_TEST (t40);
  CPPUNIT_TEST (t41);
  CPPUNIT_TEST (t42);
  CPPUNIT_TEST (t43);
  CPPUNIT_TEST (t44);
  CPPUNIT_TEST (t45);
  CPPUNIT_TEST (t46);
  CPPUNIT_TEST (t47);
  CPPUNIT_TEST_SUITE_END ();

 private:
  void do_encoder_check (bool use_encode_in_or_out,
			 const char** c_t1_in,
			 const char** c_t1_res,
			 const size_t n_io_items,
			 const size_t* n_input_items,
			 const size_t* n_output_items,
			 const size_t block_size_bits,
			 const size_t n_code_inputs,
			 const size_t n_code_outputs,
			 const int* code_generators,
			 const int* code_feedback = 0,
			 const bool do_termination = true,
			 const size_t start_state = 0,
			 const size_t term_state = 0,
			 const int encode_soai = -1);

  void t0 ();
  void t1 ();
  void t2 ();
  void t3 ();
  void t4 ();
  void t5 ();
  void t6 ();
  void t7 ();
  void t8 ();
  void t9 ();
  void t10 ();
  void t11 ();
  void t12 ();
  void t13 ();
  void t14 ();
  void t15 ();
  void t16 ();
  void t17 ();
  void t18 ();
  void t19 ();
  void t20 ();
  void t21 ();
  void t22 ();
  void t23 ();
  void t24 ();
  void t25 ();
  void t26 ();
  void t27 ();
  void t28 ();
  void t29 ();
  void t30 ();
  void t31 ();
  void t32 ();
  void t33 ();
  void t34 ();
  void t35 ();
  void t36 ();
  void t37 ();
  void t38 ();
  void t39 ();
  void t40 ();
  void t41 ();
  void t42 ();
  void t43 ();
  void t44 ();
  void t45 ();
  void t46 ();
  void t47 ();
};

#endif /* INCLUDED_QA_ENCODER_CONVOLUTIONAL_IC1_IC1_H */
