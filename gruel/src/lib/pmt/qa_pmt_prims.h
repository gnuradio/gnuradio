/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_QA_PMT_PRIMS_H
#define INCLUDED_QA_PMT_PRIMS_H

#include <gruel/attributes.h>
#include <gruel/api.h> //reason: suppress warnings
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class __GR_ATTR_EXPORT qa_pmt_prims : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_pmt_prims);
  CPPUNIT_TEST(test_symbols);
  CPPUNIT_TEST(test_booleans);
  CPPUNIT_TEST(test_integers);
  CPPUNIT_TEST(test_uint64s);
  CPPUNIT_TEST(test_reals);
  CPPUNIT_TEST(test_complexes);
  CPPUNIT_TEST(test_pairs);
  CPPUNIT_TEST(test_vectors);
  CPPUNIT_TEST(test_tuples);
  CPPUNIT_TEST(test_equivalence);
  CPPUNIT_TEST(test_misc);
  CPPUNIT_TEST(test_dict);
  CPPUNIT_TEST(test_any);
  CPPUNIT_TEST(test_msg_accepter);
  CPPUNIT_TEST(test_io);
  CPPUNIT_TEST(test_lists);
  CPPUNIT_TEST(test_serialize);
  CPPUNIT_TEST(test_sets);
  CPPUNIT_TEST(test_sugar);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_symbols();
  void test_booleans();
  void test_integers();
  void test_uint64s();
  void test_reals();
  void test_complexes();
  void test_pairs();
  void test_vectors();
  void test_tuples();
  void test_equivalence();
  void test_misc();
  void test_dict();
  void test_any();
  void test_msg_accepter();
  void test_io();
  void test_lists();
  void test_serialize();
  void test_sets();
  void test_sugar();
};

#endif /* INCLUDED_QA_PMT_PRIMS_H */

