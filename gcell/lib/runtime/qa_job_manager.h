/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_QA_JOB_MANAGER_H
#define INCLUDED_QA_JOB_MANAGER_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_job_manager;
typedef void (qa_job_manager::*test_t)();


class qa_job_manager : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_job_manager);
  CPPUNIT_TEST(t0);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST(t2);
  CPPUNIT_TEST(t3);
  CPPUNIT_TEST(t4);
  CPPUNIT_TEST(t5);
  CPPUNIT_TEST(t6);
  CPPUNIT_TEST(t7);
  CPPUNIT_TEST(t8);
  CPPUNIT_TEST(t9);
  CPPUNIT_TEST(t10);
  CPPUNIT_TEST(t11);
  CPPUNIT_TEST(t12);
  CPPUNIT_TEST(t13);
  CPPUNIT_TEST(t14);
  CPPUNIT_TEST(t15);
  CPPUNIT_TEST_SUITE_END();

 private:
  void leak_check(test_t t, const std::string &name);

  void t0();
  void t1();
  void t1_body();
  void t2();
  void t2_body();
  void t3();
  void t3_body();
  void t4();
  void t4_body();
  void t5();
  void t5_body();
  void t6();
  void t6_body();
  void t7();
  void t7_body();
  void t8();
  void t8_body();
  void t9();
  void t9_body();
  void t10();
  void t10_body();
  void t11();
  void t11_body();
  void t12();
  void t12_body();
  void t13();
  void t13_body();
  void t14();
  void t14_body();
  void t15();
  void t15_body();

};

#endif /* INCLUDED_QA_JOB_MANAGER_H */
