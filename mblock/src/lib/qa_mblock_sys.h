/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef INCLUDED_QA_MBLOCK_SYS_H
#define INCLUDED_QA_MBLOCK_SYS_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_mblock_sys : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_mblock_sys);
  CPPUNIT_TEST(test_sys_1);
  CPPUNIT_TEST(test_sys_2);
  CPPUNIT_TEST(test_bitset_1);
  CPPUNIT_TEST(test_disconnect);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_sys_1();
  void test_sys_2();
  void test_bitset_1();
  void test_disconnect();
};

#endif /* INCLUDED_QA_MBLOCK_SYS_H */

