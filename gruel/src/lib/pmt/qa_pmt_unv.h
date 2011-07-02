/* -*- c++ -*- */
/*
 * Copyright 2006,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QA_PMT_UNV_H
#define INCLUDED_QA_PMT_UNV_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_pmt_unv : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_pmt_unv);
  CPPUNIT_TEST(test_u8vector);
  CPPUNIT_TEST(test_s8vector);
  CPPUNIT_TEST(test_u16vector);
  CPPUNIT_TEST(test_s16vector);
  CPPUNIT_TEST(test_u32vector);
  CPPUNIT_TEST(test_s32vector);
  CPPUNIT_TEST(test_u64vector);
  CPPUNIT_TEST(test_s64vector);
  CPPUNIT_TEST(test_f32vector);
  CPPUNIT_TEST(test_f64vector);
  CPPUNIT_TEST(test_c32vector);
  CPPUNIT_TEST(test_c64vector);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_u8vector();
  void test_s8vector();
  void test_u16vector();
  void test_s16vector();
  void test_u32vector();
  void test_s32vector();
  void test_u64vector();
  void test_s64vector();
  void test_f32vector();
  void test_f64vector();
  void test_c32vector();
  void test_c64vector();
};

#endif
