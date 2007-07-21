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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qa_inband_packet_prims.h>
#include <cppunit/TestAssert.h>
#include <stdio.h>
#include <usrp_inband_usb_packet.h>             // will change on gigabit crossover

typedef usrp_inband_usb_packet transport_pkt;

void
qa_inband_packet_prims::test_flags()
{
  transport_pkt pkt;

  // Test each one of the flags while ensuring no other fields become set in the process
  pkt.set_header(pkt.FL_START_OF_BURST,0,0,0);
  CPPUNIT_ASSERT_EQUAL(1, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  pkt.set_header(pkt.FL_END_OF_BURST,0,0,0);
  CPPUNIT_ASSERT_EQUAL(0, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(1, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  pkt.set_header(pkt.FL_OVERRUN,0,0,0);
  CPPUNIT_ASSERT_EQUAL(0, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(1, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  pkt.set_header(pkt.FL_UNDERRUN,0,0,0);
  CPPUNIT_ASSERT_EQUAL(0, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(1, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  pkt.set_header(pkt.FL_DROPPED,0,0,0);
  CPPUNIT_ASSERT_EQUAL(0, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(1, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  // test of all fields set
  pkt.set_header(
    pkt.FL_START_OF_BURST |
    pkt.FL_END_OF_BURST |
    pkt.FL_UNDERRUN |
    pkt.FL_OVERRUN |
    pkt.FL_DROPPED 
    ,0,0,0);
  CPPUNIT_ASSERT_EQUAL(1, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(1, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(1, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(1, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(1, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());


}
//////////////////////////////////////////////////////////////////////

void
qa_inband_packet_prims::test_fields()
{
  transport_pkt pkt;
  void * payload;
  
  // test word0 field exclusiveness
  //
  // I want to test max values of each field to ensure field boundaries
  // but these max values could change based on technology?  The
  // max payload is returned by a private method so the code is not
  // technology dependent
  pkt.set_header(0,16,0,0);
  CPPUNIT_ASSERT_EQUAL(16, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());

  pkt.set_header(0,0,8,0);
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(8, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0,pkt.payload_len());

  pkt.set_header(0,0,0,pkt.max_payload());  
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(pkt.max_payload(), pkt.payload_len());

  // test timestamp, shouldn't have to test other fields since
  // setting the timestamp only has the ability to affect one word
  pkt.set_timestamp(54);
  CPPUNIT_ASSERT_EQUAL(uint32_t(54), pkt.timestamp());

  // test the payload, ensure no other fields overwritten
  //
  // is there a better test for this?
  pkt.set_header(0,0,0,0);
  payload = malloc(pkt.payload_len());
  memset(payload, 'f', pkt.payload_len());
  memcpy(pkt.payload(), payload, pkt.payload_len());
  CPPUNIT_ASSERT_EQUAL(0, memcmp(pkt.payload(), payload, pkt.payload_len()));
  CPPUNIT_ASSERT_EQUAL(0, pkt.start_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.end_of_burst());
  CPPUNIT_ASSERT_EQUAL(0, pkt.overrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.underrun());
  CPPUNIT_ASSERT_EQUAL(0, pkt.dropped());
  CPPUNIT_ASSERT_EQUAL(0, pkt.chan());
  CPPUNIT_ASSERT_EQUAL(0, pkt.tag());
  CPPUNIT_ASSERT_EQUAL(0, pkt.payload_len());
  free(payload);

}
//////////////////////////////////////////////////////////////////////
