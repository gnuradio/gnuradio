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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_QA_GR_TOP_BLOCK_H
#define INCLUDED_QA_GR_TOP_BLOCK_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <stdexcept>

class qa_top_block : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(qa_top_block);

  CPPUNIT_TEST(t0);
  CPPUNIT_TEST(t1_run);
  CPPUNIT_TEST(t2_start_stop_wait);
  CPPUNIT_TEST(t3_lock_unlock);
  CPPUNIT_TEST(t4_reconfigure);  // triggers 'join never returns' bug
  CPPUNIT_TEST(t5_max_noutputs);
  CPPUNIT_TEST(t6_reconfig_max_noutputs);
  CPPUNIT_TEST(t7_max_noutputs_per_block);
  CPPUNIT_TEST(t8_reconfig_max_noutputs_per_block);
  CPPUNIT_TEST(t9_max_output_buffer);
  CPPUNIT_TEST(t10_reconfig_max_output_buffer);
  CPPUNIT_TEST(t11_set_block_affinity);

  CPPUNIT_TEST_SUITE_END();

private:

  void t0();
  void t1_run();
  void t2_start_stop_wait();
  void t3_lock_unlock();
  void t4_reconfigure();
  void t5_max_noutputs();
  void t6_reconfig_max_noutputs();
  void t7_max_noutputs_per_block();
  void t8_reconfig_max_noutputs_per_block();
  void t9_max_output_buffer();
  void t10_reconfig_max_output_buffer();
  void t11_set_block_affinity();

};

#endif /* INCLUDED_QA_GR_TOP_BLOCK_H */
