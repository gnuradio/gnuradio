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

#ifndef _QA_ATSC_DATA_INTERLEAVER_H_
#define _QA_ATSC_DATA_INTERLEAVER_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <stdio.h>

#include <atsci_data_interleaver.h>

class qa_atsci_data_interleaver : public CppUnit::TestCase {
 public:

  void setUp (){
    outbound.reset ();
    inbound.reset ();
  }

  CPPUNIT_TEST_SUITE (qa_atsci_data_interleaver);
  CPPUNIT_TEST (t0);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST_SUITE_END ();

 private:
  atsci_data_interleaver		outbound;
  atsci_data_deinterleaver	inbound;
  
  void t0 ();
  void t1 ();

  void init_test_packet (int counter, atsc_mpeg_packet_rs_encoded &out);
  void print_packet (FILE *fp, int frame_number, int field_number, int segment_number,
		     const atsc_mpeg_packet_rs_encoded &in);
  void chk_assert (const atsc_mpeg_packet_rs_encoded &expected,
		   const atsc_mpeg_packet_rs_encoded &actual);
};


#endif /* _QA_ATSC_DATA_INTERLEAVER_H_ */
