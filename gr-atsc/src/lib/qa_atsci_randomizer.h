/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#ifndef _QA_ATSC_RANDOMIZER_H_
#define _QA_ATSC_RANDOMIZER_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#include <atsci_randomizer.h>

class qa_atsci_randomizer : public CppUnit::TestCase {
 private:
  atsci_randomizer	randomizer;
  
 public:

  void setUp (){
    // nop
  }

  void tearDown (){
    // nop
  }

  CPPUNIT_TEST_SUITE (qa_atsci_randomizer);
  CPPUNIT_TEST (t0_compare_output_maps);
  CPPUNIT_TEST (t1_initial_states);
  CPPUNIT_TEST (t2_initial_values);
  CPPUNIT_TEST (t3_reset);
  CPPUNIT_TEST (t4_high_level);
  CPPUNIT_TEST_SUITE_END ();


 private:
  void t0_compare_output_maps ();
  void t1_initial_states ();
  void t2_initial_values ();
  void t3_reset ();
  void t4_high_level ();
  
};

#endif /* _QA_ATSC_RANDOMIZER_H_ */
