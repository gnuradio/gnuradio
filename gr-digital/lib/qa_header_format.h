/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
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

#ifndef _QA_DIGITAL_HEADER_FORMAT_H_
#define _QA_DIGITAL_HEADER_FORMAT_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_header_format : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(qa_header_format);
  CPPUNIT_TEST(test_default_format);
  CPPUNIT_TEST(test_default_parse);
  CPPUNIT_TEST(test_counter_format);
  CPPUNIT_TEST(test_counter_parse);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_default_format();
  void test_default_parse();
  void test_default_parse_soft();
  void test_counter_format();
  void test_counter_parse();
  void test_counter_parse_soft();
};

#endif /* _QA_DIGITAL_HEADER_FORMAT_H_ */
