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

#ifndef INCLUDED_QA_GR_FLOWGRAPH_H
#define INCLUDED_QA_GR_FLOWGRAPH_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <stdexcept>

class qa_gr_flowgraph : public CppUnit::TestCase 
{
  CPPUNIT_TEST_SUITE(qa_gr_flowgraph);
  
  CPPUNIT_TEST(t0);
  CPPUNIT_TEST(t1_connect);
  CPPUNIT_TEST(t2_connect_invalid_src_port_neg);
  CPPUNIT_TEST(t3_connect_src_port_exceeds);
  CPPUNIT_TEST(t4_connect_invalid_dst_port_neg);
  CPPUNIT_TEST(t5_connect_dst_port_exceeds);
  CPPUNIT_TEST(t6_connect_dst_in_use);
  CPPUNIT_TEST(t7_connect_one_src_two_dst);
  CPPUNIT_TEST(t8_connect_type_mismatch);
  CPPUNIT_TEST(t9_disconnect);
  CPPUNIT_TEST(t10_disconnect_unconnected_block);
  CPPUNIT_TEST(t11_disconnect_unconnected_port);
  CPPUNIT_TEST(t12_validate);
  CPPUNIT_TEST(t13_validate_missing_input_assignment);
  CPPUNIT_TEST(t14_validate_missing_output_assignment);
  CPPUNIT_TEST(t15_clear);
  CPPUNIT_TEST(t16_partition);

  CPPUNIT_TEST_SUITE_END();

private:

  void t0();
  void t1_connect();
  void t2_connect_invalid_src_port_neg();
  void t3_connect_src_port_exceeds();
  void t4_connect_invalid_dst_port_neg();
  void t5_connect_dst_port_exceeds();
  void t6_connect_dst_in_use();
  void t7_connect_one_src_two_dst();
  void t8_connect_type_mismatch();
  void t9_disconnect();
  void t10_disconnect_unconnected_block();
  void t11_disconnect_unconnected_port();
  void t12_validate();
  void t13_validate_missing_input_assignment();
  void t14_validate_missing_output_assignment();
  void t15_clear();
  void t16_partition();
};

#endif /* INCLUDED_QA_GR_FLOWGRAPH_H */
