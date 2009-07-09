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

#ifndef QA_INBAND_PACKET_PRIMS_H
#define QA_INBAND_PACKET_PRIMS_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

class qa_inband_packet_prims : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_inband_packet_prims);
  CPPUNIT_TEST(test_flags);
  CPPUNIT_TEST(test_fields);
  CPPUNIT_TEST_SUITE_END();

 private:
  void test_flags();
  void test_fields();

};

#endif /* INCLUDED_QA_INBAND_PACKET_PRIMS_H */
