/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#ifndef _QA_DIGITAL_HEADER_BUFFER_H_
#define _QA_DIGITAL_HEADER_BUFFER_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_header_buffer : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(qa_header_buffer);
  CPPUNIT_TEST(test_add8);
  CPPUNIT_TEST(test_add16);
  CPPUNIT_TEST(test_add32);
  CPPUNIT_TEST(test_add64);
  CPPUNIT_TEST(test_add_many);

  CPPUNIT_TEST(test_extract8);
  CPPUNIT_TEST(test_extract16);
  CPPUNIT_TEST(test_extract32);
  CPPUNIT_TEST(test_extract64);
  CPPUNIT_TEST(test_extract_many);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_add8();
  void test_add16();
  void test_add32();
  void test_add64();
  void test_add_many();

  void test_extract8();
  void test_extract16();
  void test_extract32();
  void test_extract64();
  void test_extract_many();
};

#endif /* _QA_DIGITAL_HEADER_BUFFER_H_ */
