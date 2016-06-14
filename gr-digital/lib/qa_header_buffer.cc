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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <volk/volk.h>
#include <gnuradio/attributes.h>

#include <stdio.h>
#include <cppunit/TestAssert.h>

#include "qa_header_buffer.h"
#include <gnuradio/digital/header_buffer.h>

void
qa_header_buffer::test_add8()
{
  size_t len = sizeof(uint8_t);
  uint8_t *buf = (uint8_t*)volk_malloc(len, volk_get_alignment());

  gr::digital::header_buffer header(buf);
  header.add_field8(0xAF);

  CPPUNIT_ASSERT_EQUAL(len, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[0]);

  header.clear();
  CPPUNIT_ASSERT_EQUAL((size_t)0, header.length());

  volk_free(buf);
}

void
qa_header_buffer::test_add16()
{
  size_t len = sizeof(uint16_t);
  uint8_t *buf = (uint8_t*)volk_malloc(len, volk_get_alignment());

  uint16_t data = 0xAF5C;

  gr::digital::header_buffer header(buf);
  header.add_field16(data);

  // Test standard add of a uint16
  CPPUNIT_ASSERT_EQUAL(len, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[1]);

  // Clear; test to make sure it's clear
  header.clear();
  CPPUNIT_ASSERT_EQUAL((size_t)0, header.length());

  // Test adding some subset of bits (must be a byte boundary)
  header.add_field16(data, 8);
  CPPUNIT_ASSERT_EQUAL((size_t)1, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[0]);
  header.clear();

  // Test adding and byte swapping
  header.add_field16(data, 16, true);
  CPPUNIT_ASSERT_EQUAL((size_t)2, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[1]);
  header.clear();

  // Test adding some subset of bits and byte swapping
  header.add_field16(data, 8, true);
  CPPUNIT_ASSERT_EQUAL((size_t)1, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[0]);
  header.clear();

  volk_free(buf);
}

void
qa_header_buffer::test_add32()
{
  size_t len = sizeof(uint32_t);
  uint8_t *buf = (uint8_t*)volk_malloc(len, volk_get_alignment());

  uint32_t data = 0xAF5C7654;

  gr::digital::header_buffer header(buf);
  header.add_field32(data);

  // Test standard add of a uint32
  CPPUNIT_ASSERT_EQUAL(len, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[3]);

  // Clear; test to make sure it's clear
  header.clear();
  CPPUNIT_ASSERT_EQUAL((size_t)0, header.length());

  // Test adding some subset of bits (must be a byte boundary)
  header.add_field32(data, 8);
  CPPUNIT_ASSERT_EQUAL((size_t)1, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[0]);
  header.clear();

  // Test adding and byte swapping
  header.add_field32(data, 32, true);
  CPPUNIT_ASSERT_EQUAL((size_t)4, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[3]);
  header.clear();

  // Test adding some subset of bits and byte swapping
  header.add_field32(data, 24, true);
  CPPUNIT_ASSERT_EQUAL((size_t)3, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[2]);
  header.clear();

  volk_free(buf);
}

void
qa_header_buffer::test_add64()
{
  size_t len = sizeof(uint64_t);
  uint8_t *buf = (uint8_t*)volk_malloc(len, volk_get_alignment());

  uint64_t data = 0xAF5C765432104567;

  gr::digital::header_buffer header(buf);
  header.add_field64(data);

  // Test standard add of a uint64
  CPPUNIT_ASSERT_EQUAL(len, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[3]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x32, header.header()[4]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x10, header.header()[5]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[6]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x67, header.header()[7]);

  // Clear; test to make sure it's clear
  header.clear();
  CPPUNIT_ASSERT_EQUAL((size_t)0, header.length());

  // Test adding some subset of bits (must be a byte boundary)
  header.add_field64(data, 48);
  CPPUNIT_ASSERT_EQUAL((size_t)6, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x32, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x10, header.header()[3]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[4]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x67, header.header()[5]);
  header.clear();

  // Test adding and byte swapping
  header.add_field64(data, 64, true);
  CPPUNIT_ASSERT_EQUAL((size_t)8, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x67, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x10, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x32, header.header()[3]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[4]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[5]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x5C, header.header()[6]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAF, header.header()[7]);
  header.clear();

  // Test adding some subset of bits and byte swapping
  header.add_field64(data, 40, true);
  CPPUNIT_ASSERT_EQUAL((size_t)5, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x67, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x10, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x32, header.header()[3]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[4]);
  header.clear();

  volk_free(buf);
}

void
qa_header_buffer::test_add_many()
{
  size_t len = (32+64+8+16+32)/8;
  uint8_t *buf = (uint8_t*)volk_malloc(len, volk_get_alignment());

  gr::digital::header_buffer header(buf);
  header.add_field32(0x01234567);
  header.add_field64(0x89ABCDEFFEDCBA98);
  header.add_field8(0x76);
  header.add_field16(0x5432);
  header.add_field32(0x10012345);

  CPPUNIT_ASSERT_EQUAL(len, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x01, header.header()[0]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x23, header.header()[1]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[2]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x67, header.header()[3]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x89, header.header()[4]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xAB, header.header()[5]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xCD, header.header()[6]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xEF, header.header()[7]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xFE, header.header()[8]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xDC, header.header()[9]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0xBA, header.header()[10]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x98, header.header()[11]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x76, header.header()[12]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x54, header.header()[13]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x32, header.header()[14]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x10, header.header()[15]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x01, header.header()[16]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x23, header.header()[17]);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x45, header.header()[18]);
}

void
qa_header_buffer::test_extract8()
{
  gr::digital::header_buffer header;

  uint64_t data = 0x0123456701234567;

  // Packed format: 0x80C4A2E680C4A2E6

  volk_64u_byteswap(&data, 1);
  for(int i = 0; i < 64; i++) {
    header.insert_bit((data >> i) & 0x01);
  }

  uint8_t x0 = header.extract_field8(0);
  uint8_t x1 = header.extract_field8(12, 8);
  uint8_t x2 = header.extract_field8(12, 4);

  CPPUNIT_ASSERT_EQUAL((size_t)64, header.length());
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x80, x0);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x4A, x1);
  CPPUNIT_ASSERT_EQUAL((uint8_t)0x04, x2);
}

void
qa_header_buffer::test_extract16()
{
  gr::digital::header_buffer header;

  uint64_t data = 0x0123456701234567;

  // Packed format: 0x80C4A2E680C4A2E6

  volk_64u_byteswap(&data, 1);
  for(int i = 0; i < 64; i++) {
    header.insert_bit((data >> i) & 0x01);
  }

  uint16_t x0 = header.extract_field16(0);
  uint16_t x1 = header.extract_field16(12, 16);
  uint16_t x2 = header.extract_field16(12, 12);

  CPPUNIT_ASSERT_EQUAL((size_t)64, header.length());
  CPPUNIT_ASSERT_EQUAL((uint16_t)0x80C4, x0);
  CPPUNIT_ASSERT_EQUAL((uint16_t)0x4A2E, x1);
  CPPUNIT_ASSERT_EQUAL((uint16_t)0x04A2, x2);
}

void
qa_header_buffer::test_extract32()
{
  gr::digital::header_buffer header;

  uint64_t data = 0x0123456701234567;

  // Packed format: 0x80C4A2E680C4A2E6

  volk_64u_byteswap(&data, 1);
  for(int i = 0; i < 64; i++) {
    header.insert_bit((data >> i) & 0x01);
  }

  uint32_t x0 = header.extract_field32(0);
  uint32_t x1 = header.extract_field32(12, 32);
  uint32_t x2 = header.extract_field32(12, 24);

  CPPUNIT_ASSERT_EQUAL((size_t)64, header.length());
  CPPUNIT_ASSERT_EQUAL((uint32_t)0x80C4A2E6, x0);
  CPPUNIT_ASSERT_EQUAL((uint32_t)0x4A2E680C, x1);
  CPPUNIT_ASSERT_EQUAL((uint32_t)0x004A2E68, x2);
}

void
qa_header_buffer::test_extract64()
{
  gr::digital::header_buffer header;

  uint64_t data = 0x0123456701234567;

  // Packed format: 0x80C4A2E680C4A2E6

  volk_64u_byteswap(&data, 1);
  for(int i = 0; i < 64; i++) {
    header.insert_bit((data >> i) & 0x01);
  }

  uint64_t x0 = header.extract_field64(0);
  uint64_t x1 = header.extract_field64(0, 32);
  uint64_t x2 = header.extract_field64(0, 44);

  CPPUNIT_ASSERT_EQUAL((size_t)64, header.length());
  CPPUNIT_ASSERT_EQUAL((uint64_t)0x80C4A2E680C4A2E6, x0);
  CPPUNIT_ASSERT_EQUAL((uint64_t)0x0000000080C4A2E6, x1);
  CPPUNIT_ASSERT_EQUAL((uint64_t)0x0000080C4A2E680C, x2);
}

void
qa_header_buffer::test_extract_many()
{
  gr::digital::header_buffer header;

  uint64_t data = 0x0123456701234567;

  // Packed format: 0x80C4A2E680C4A2E6

  volk_64u_byteswap(&data, 1);
  for(int i = 0; i < 64; i++) {
    header.insert_bit((data >> i) & 0x01);
  }

  uint64_t x0 = header.extract_field64(0);
  uint16_t x1 = header.extract_field16(28, 12);
  uint32_t x2 = header.extract_field32(40, 21);
  uint16_t x3 = header.extract_field16(1, 12);
  uint8_t  x4 = header.extract_field8 (7, 5);

  CPPUNIT_ASSERT_EQUAL((size_t)64, header.length());
  CPPUNIT_ASSERT_EQUAL((uint64_t)0x80C4A2E680C4A2E6, x0);
  CPPUNIT_ASSERT_EQUAL((uint16_t)0x0680, x1);
  CPPUNIT_ASSERT_EQUAL((uint32_t)0x0018945C, x2);
  CPPUNIT_ASSERT_EQUAL((uint16_t)0x0018, x3);
  CPPUNIT_ASSERT_EQUAL((uint8_t) 0x0C, x4);
}
