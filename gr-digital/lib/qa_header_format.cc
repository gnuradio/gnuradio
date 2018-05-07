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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/attributes.h>
#include <gnuradio/digital/header_format_counter.h>
#include <gnuradio/digital/header_format_default.h>
#include <gnuradio/blocks/unpack_k_bits.h>
#include <gnuradio/expj.h>
#include <volk/volk.h>
#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <cmath>

BOOST_AUTO_TEST_CASE(test_default_format)
{
  static const int N = 4800;
  int upper8 = (N >> 8) & 0xFF;
  int lower8 = N & 0xFF;

  std::string ac = "1010101010101010"; //0xAAAA
  unsigned char *data = (unsigned char*)volk_malloc(N*sizeof(unsigned char),
                                                    volk_get_alignment());
  srand (time(NULL));
  for(unsigned int i = 0; i < N; i++) {
    data[i] = rand() % 256;
  }

  gr::digital::header_format_default::sptr hdr_format;
  hdr_format = gr::digital::header_format_default::make(ac, 0);

  pmt::pmt_t output;
  pmt::pmt_t info = pmt::make_dict();

  bool ret = hdr_format->format(N, data, output, info);
  size_t length = pmt::length(output);

  BOOST_REQUIRE(ret);
  BOOST_REQUIRE_EQUAL(length, hdr_format->header_nbytes());
  BOOST_REQUIRE_EQUAL(8*length, hdr_format->header_nbits());

  // Test access code formatted correctly
  unsigned char h0 = pmt::u8vector_ref(output, 0);
  unsigned char h1 = pmt::u8vector_ref(output, 1);
  BOOST_REQUIRE_EQUAL(0xAA, (int)h0);
  BOOST_REQUIRE_EQUAL(0xAA, (int)h1);

  // Test upper and lower portion of length field, repeated twice
  unsigned char h2 = pmt::u8vector_ref(output, 2);
  unsigned char h3 = pmt::u8vector_ref(output, 3);
  unsigned char h4 = pmt::u8vector_ref(output, 4);
  unsigned char h5 = pmt::u8vector_ref(output, 5);
  BOOST_REQUIRE_EQUAL(upper8, (int)h2);
  BOOST_REQUIRE_EQUAL(lower8, (int)h3);
  BOOST_REQUIRE_EQUAL(upper8, (int)h4);
  BOOST_REQUIRE_EQUAL(lower8, (int)h5);

  volk_free(data);
}


BOOST_AUTO_TEST_CASE(test_default_parse)
{
  static const int nbytes = 106;
  static const int nbits = 8*nbytes;
  unsigned char *bytes = (unsigned char*)volk_malloc(nbytes*sizeof(unsigned char),
                                                     volk_get_alignment());
  unsigned char *bits = (unsigned char*)volk_malloc(nbits*sizeof(unsigned char),
                                                    volk_get_alignment());

  srand(time(NULL));

  // Fill bytes with random values
  for(unsigned int i = 0; i < nbytes; i++) {
    bytes[i] = rand() % 256;
  }

  int index = 0;
  bytes[index+0] = 0xAA;
  bytes[index+1] = 0xAA;
  bytes[index+2] = 0x00;
  bytes[index+3] = 0x64;
  bytes[index+4] = 0x00;
  bytes[index+5] = 0x64;

  gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
  unpacker.unpack(bits, bytes, nbytes);

  std::string ac = "1010101010101010"; //0xAAAA
  gr::digital::header_format_default::sptr hdr_format;
  hdr_format = gr::digital::header_format_default::make(ac, 0);

  int count = 0;
  std::vector<pmt::pmt_t> info;
  bool ret = hdr_format->parse(nbits, bits, info, count);

  BOOST_REQUIRE(ret);
  BOOST_REQUIRE_EQUAL((size_t)1, info.size());

  pmt::pmt_t dict = info[0];
  int payload_bits = pmt::to_long(pmt::dict_ref(dict, pmt::intern("payload symbols"),
                                                pmt::from_long(-1)));

  int hdr_bits = (int)hdr_format->header_nbits();
  int expected_bits = nbits - hdr_bits;
  BOOST_REQUIRE_EQUAL(expected_bits, payload_bits);

  volk_free(bytes);
  volk_free(bits);
}

BOOST_AUTO_TEST_CASE(test_counter_format)
{
  static const int N = 4800;
  int upper8 = (N >> 8) & 0xFF;
  int lower8 = N & 0xFF;

  std::string ac = "1010101010101010"; //0xAAAA
  unsigned char *data = (unsigned char*)volk_malloc(N*sizeof(unsigned char),
                                                    volk_get_alignment());
  srand (time(NULL));
  for(unsigned int i = 0; i < N; i++) {
    data[i] = rand() % 256;
  }

  uint16_t expected_bps = 2;
  gr::digital::header_format_counter::sptr hdr_format;
  hdr_format = gr::digital::header_format_counter::make(ac, 0, expected_bps);

  pmt::pmt_t output;
  pmt::pmt_t info = pmt::make_dict();

  bool ret = hdr_format->format(N, data, output, info);
  size_t length = pmt::length(output);

  BOOST_REQUIRE(ret);
  BOOST_REQUIRE_EQUAL(length, hdr_format->header_nbytes());
  BOOST_REQUIRE_EQUAL(8*length, hdr_format->header_nbits());

  // Test access code formatted correctly
  unsigned char h0 = pmt::u8vector_ref(output, 0);
  unsigned char h1 = pmt::u8vector_ref(output, 1);
  BOOST_REQUIRE_EQUAL(0xAA, (int)h0);
  BOOST_REQUIRE_EQUAL(0xAA, (int)h1);

  // Test upper and lower portion of length field, repeated twice
  unsigned char h2 = pmt::u8vector_ref(output, 2);
  unsigned char h3 = pmt::u8vector_ref(output, 3);
  unsigned char h4 = pmt::u8vector_ref(output, 4);
  unsigned char h5 = pmt::u8vector_ref(output, 5);
  BOOST_REQUIRE_EQUAL(upper8, (int)h2);
  BOOST_REQUIRE_EQUAL(lower8, (int)h3);
  BOOST_REQUIRE_EQUAL(upper8, (int)h4);
  BOOST_REQUIRE_EQUAL(lower8, (int)h5);

  uint16_t h6 = (uint16_t)pmt::u8vector_ref(output, 6);
  uint16_t h7 = (uint16_t)pmt::u8vector_ref(output, 7);
  uint16_t bps = ((h6 << 8) & 0xFF00) | (h7 & 0x00FF);
  BOOST_REQUIRE_EQUAL(expected_bps, bps);

  uint16_t h8 = pmt::u8vector_ref(output, 8);
  uint16_t h9 = pmt::u8vector_ref(output, 9);
  uint16_t counter = ((h8 << 8) & 0xFF00) | (h9 & 0x00FF);
  BOOST_REQUIRE_EQUAL((uint16_t)0, counter);

  // Run another format to increment the counter to 1 and test.
  ret = hdr_format->format(N, data, output, info);
  h8 = pmt::u8vector_ref(output, 8);
  h9 = pmt::u8vector_ref(output, 9);
  counter = ((h8 << 8) & 0xFF00) | (h9 & 0x00FF);
  BOOST_REQUIRE_EQUAL((uint16_t)1, counter);

  volk_free(data);
}


BOOST_AUTO_TEST_CASE(test_counter_parse)
{
  static const int nbytes = 110;
  static const int nbits = 8*nbytes;
  unsigned char *bytes = (unsigned char*)volk_malloc(nbytes*sizeof(unsigned char),
                                                     volk_get_alignment());
  unsigned char *bits = (unsigned char*)volk_malloc(nbits*sizeof(unsigned char),
                                                    volk_get_alignment());

  srand(time(NULL));

  // Fill bytes with random values
  for(unsigned int i = 0; i < nbytes; i++) {
    bytes[i] = rand() % 256;
  }

  int index = 0;
  bytes[index+0] = 0xAA;
  bytes[index+1] = 0xAA;
  bytes[index+2] = 0x00;
  bytes[index+3] = 0x64;
  bytes[index+4] = 0x00;
  bytes[index+5] = 0x64;
  bytes[index+6] = 0x00;
  bytes[index+7] = 0x02;
  bytes[index+8] = 0x00;
  bytes[index+9] = 0x00;

  gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
  unpacker.unpack(bits, bytes, nbytes);

  uint16_t expected_bps = 2;
  std::string ac = "1010101010101010"; //0xAAAA
  gr::digital::header_format_counter::sptr hdr_format;
  hdr_format = gr::digital::header_format_counter::make(ac, 0, expected_bps);

  int count = 0;
  std::vector<pmt::pmt_t> info;
  bool ret = hdr_format->parse(nbits, bits, info, count);

  BOOST_REQUIRE(ret);
  BOOST_REQUIRE_EQUAL((size_t)1, info.size());

  pmt::pmt_t dict = info[0];
  int payload_syms = pmt::to_long(pmt::dict_ref(dict, pmt::intern("payload symbols"),
                                                pmt::from_long(-1)));
  int bps = pmt::to_long(pmt::dict_ref(dict, pmt::intern("bps"),
                                       pmt::from_long(-1)));
  int counter = pmt::to_long(pmt::dict_ref(dict, pmt::intern("counter"),
                                           pmt::from_long(-1)));

  int hdr_bits = (int)hdr_format->header_nbits();
  int expected_bits = nbits - hdr_bits;
  BOOST_REQUIRE_EQUAL(expected_bits, payload_syms * bps);
  BOOST_REQUIRE_EQUAL(expected_bps, (uint16_t)bps);
  BOOST_REQUIRE_EQUAL(0, counter);

  volk_free(bytes);
  volk_free(bits);
}
