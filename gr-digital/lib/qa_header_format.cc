/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/attributes.h>
#include <gnuradio/blocks/unpack_k_bits.h>
#include <gnuradio/digital/header_format_counter.h>
#include <gnuradio/digital/header_format_default.h>
#include <gnuradio/expj.h>
#include <gnuradio/xoroshiro128p.h>
#include <volk/volk_alloc.hh>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <cstdio>

static void
xoroshiro_fill_buffer(uint8_t* buffer, unsigned int length, uint64_t seed = 42)
{
    uint64_t rng_state[2];
    xoroshiro128p_seed(rng_state, seed);
    uint64_t* data_ptr = reinterpret_cast<uint64_t*>(buffer);
    for (unsigned int i = 0; i < length / 8; ++i) {
        *data_ptr = xoroshiro128p_next(rng_state);
        data_ptr++;
    }
    if (length % 8) {
        uint64_t tmp = xoroshiro128p_next(rng_state);
        uint8_t* tmpptr = reinterpret_cast<uint8_t*>(&tmp);
        for (unsigned int counter = length - length % 8; counter < length; ++counter) {
            buffer[counter] = *tmpptr;
            ++tmpptr;
        }
    }
}

BOOST_AUTO_TEST_CASE(test_default_format)
{
    static const int N = 4800; /* multiple of 8 for easy random generation */
    int upper8 = (N >> 8) & 0xFF;
    int lower8 = N & 0xFF;

    std::string ac = "1010101010101010"; // 0xAAAA
    volk::vector<unsigned char> data(N);
    xoroshiro_fill_buffer(data.data(), N);
    gr::digital::header_format_default::sptr hdr_format;
    hdr_format = gr::digital::header_format_default::make(ac, 0);

    pmt::pmt_t output;
    pmt::pmt_t info = pmt::make_dict();

    bool ret = hdr_format->format(N, data.data(), output, info);
    size_t length = pmt::length(output);

    BOOST_REQUIRE(ret);
    BOOST_REQUIRE_EQUAL(length, hdr_format->header_nbytes());
    BOOST_REQUIRE_EQUAL(8 * length, hdr_format->header_nbits());

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
}


BOOST_AUTO_TEST_CASE(test_default_parse)
{
    static const int nbytes = 106;
    static const int nbits = 8 * nbytes;

    volk::vector<unsigned char> bytes(nbytes);
    volk::vector<unsigned char> bits(nbits);

    xoroshiro_fill_buffer(bytes.data(), nbytes);

    int index = 0;
    bytes[index + 0] = 0xAA;
    bytes[index + 1] = 0xAA;
    bytes[index + 2] = 0x00;
    bytes[index + 3] = 0x64;
    bytes[index + 4] = 0x00;
    bytes[index + 5] = 0x64;

    gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
    unpacker.unpack(bits.data(), bytes.data(), nbytes);

    std::string ac = "1010101010101010"; // 0xAAAA
    gr::digital::header_format_default::sptr hdr_format;
    hdr_format = gr::digital::header_format_default::make(ac, 0);

    int count = 0;
    std::vector<pmt::pmt_t> info;
    bool ret = hdr_format->parse(nbits, bits.data(), info, count);

    BOOST_REQUIRE(ret);
    BOOST_REQUIRE_EQUAL((size_t)1, info.size());

    pmt::pmt_t dict = info[0];
    int payload_bits = pmt::to_long(
        pmt::dict_ref(dict, pmt::intern("payload symbols"), pmt::from_long(-1)));

    int hdr_bits = (int)hdr_format->header_nbits();
    int expected_bits = nbits - hdr_bits;
    BOOST_REQUIRE_EQUAL(expected_bits, payload_bits);
}

BOOST_AUTO_TEST_CASE(test_counter_format)
{
    static const int N = 4800;
    int upper8 = (N >> 8) & 0xFF;
    int lower8 = N & 0xFF;

    std::string ac = "1010101010101010"; // 0xAAAA
    volk::vector<unsigned char> data(N);
    xoroshiro_fill_buffer(data.data(), N);

    uint16_t expected_bps = 2;
    gr::digital::header_format_counter::sptr hdr_format;
    hdr_format = gr::digital::header_format_counter::make(ac, 0, expected_bps);

    pmt::pmt_t output;
    pmt::pmt_t info = pmt::make_dict();

    bool ret = hdr_format->format(N, data.data(), output, info);
    size_t length = pmt::length(output);

    BOOST_REQUIRE(ret);
    BOOST_REQUIRE_EQUAL(length, hdr_format->header_nbytes());
    BOOST_REQUIRE_EQUAL(8 * length, hdr_format->header_nbits());

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
    ret = hdr_format->format(N, data.data(), output, info);
    h8 = pmt::u8vector_ref(output, 8);
    h9 = pmt::u8vector_ref(output, 9);
    counter = ((h8 << 8) & 0xFF00) | (h9 & 0x00FF);
    BOOST_REQUIRE_EQUAL((uint16_t)1, counter);
}


BOOST_AUTO_TEST_CASE(test_counter_parse)
{
    static const int nbytes = 110;
    static const int nbits = 8 * nbytes;
    volk::vector<unsigned char> bytes(nbytes);
    volk::vector<unsigned char> bits(nbits);
    xoroshiro_fill_buffer(bytes.data(), nbytes);
    int index = 0;
    bytes[index + 0] = 0xAA;
    bytes[index + 1] = 0xAA;
    bytes[index + 2] = 0x00;
    bytes[index + 3] = 0x64;
    bytes[index + 4] = 0x00;
    bytes[index + 5] = 0x64;
    bytes[index + 6] = 0x00;
    bytes[index + 7] = 0x02;
    bytes[index + 8] = 0x00;
    bytes[index + 9] = 0x00;

    gr::blocks::kernel::unpack_k_bits unpacker = gr::blocks::kernel::unpack_k_bits(8);
    unpacker.unpack(bits.data(), bytes.data(), nbytes);

    uint16_t expected_bps = 2;
    std::string ac = "1010101010101010"; // 0xAAAA
    gr::digital::header_format_counter::sptr hdr_format;
    hdr_format = gr::digital::header_format_counter::make(ac, 0, expected_bps);

    int count = 0;
    std::vector<pmt::pmt_t> info;
    bool ret = hdr_format->parse(nbits, bits.data(), info, count);

    BOOST_REQUIRE(ret);
    BOOST_REQUIRE_EQUAL((size_t)1, info.size());

    pmt::pmt_t dict = info[0];
    int payload_syms = pmt::to_long(
        pmt::dict_ref(dict, pmt::intern("payload symbols"), pmt::from_long(-1)));
    int bps = pmt::to_long(pmt::dict_ref(dict, pmt::intern("bps"), pmt::from_long(-1)));
    int counter =
        pmt::to_long(pmt::dict_ref(dict, pmt::intern("counter"), pmt::from_long(-1)));

    int hdr_bits = (int)hdr_format->header_nbits();
    int expected_bits = nbits - hdr_bits;
    BOOST_REQUIRE_EQUAL(expected_bits, payload_syms * bps);
    BOOST_REQUIRE_EQUAL(expected_bps, (uint16_t)bps);
    BOOST_REQUIRE_EQUAL(0, counter);
}
