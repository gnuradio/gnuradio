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
  CPPUNIT_TEST_SUITE_END ();

 private:
  void do_encoder_check (const char** c_t1_in,
			 const char** c_t1_res,
			 int n_output_items,
			 int block_size_bits,
			 int n_code_inputs,
			 int n_code_outputs,
			 const int* code_generators,
			 const int* code_feedback = 0);

  void t0 ();
  void t1 ();
  void t2 ();
  void t3 ();
  void t4 ();
  void t5 ();
  void t6 ();
  void t7 ();
  void t8 ();
};

#endif /* INCLUDED_QA_ENCODER_CONVOLUTIONAL_IC1_IC1_H */
