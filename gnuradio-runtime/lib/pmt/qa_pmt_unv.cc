/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/messages/msg_passing.h>
#include <pmt/api.h> //reason: suppress warnings
#include <pmt/pmt.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_u8vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_u8vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    uint8_t s0 = uint8_t(10);
    uint8_t s1 = uint8_t(20);
    uint8_t s2 = uint8_t(30);

    pmt::u8vector_set(v1, 0, s0);
    pmt::u8vector_set(v1, 1, s1);
    pmt::u8vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::u8vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::u8vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::u8vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::u8vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::u8vector_set(v1, N, uint8_t(0)), pmt::out_of_range);

    size_t len;
    const uint8_t* rd = pmt::u8vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    uint8_t* wr = pmt::u8vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = uint8_t(0);
    BOOST_CHECK_EQUAL(uint8_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_s8vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_s8vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    int8_t s0 = int8_t(10);
    int8_t s1 = int8_t(20);
    int8_t s2 = int8_t(30);

    pmt::s8vector_set(v1, 0, s0);
    pmt::s8vector_set(v1, 1, s1);
    pmt::s8vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::s8vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::s8vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::s8vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::s8vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::s8vector_set(v1, N, int8_t(0)), pmt::out_of_range);

    size_t len;
    const int8_t* rd = pmt::s8vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    int8_t* wr = pmt::s8vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = int8_t(0);
    BOOST_CHECK_EQUAL(int8_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_u16vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_u16vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    uint16_t s0 = uint16_t(10);
    uint16_t s1 = uint16_t(20);
    uint16_t s2 = uint16_t(30);

    pmt::u16vector_set(v1, 0, s0);
    pmt::u16vector_set(v1, 1, s1);
    pmt::u16vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::u16vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::u16vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::u16vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::u16vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::u16vector_set(v1, N, uint16_t(0)), pmt::out_of_range);

    size_t len;
    const uint16_t* rd = pmt::u16vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    uint16_t* wr = pmt::u16vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = uint16_t(0);
    BOOST_CHECK_EQUAL(uint16_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_s16vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_s16vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    int16_t s0 = int16_t(10);
    int16_t s1 = int16_t(20);
    int16_t s2 = int16_t(30);

    pmt::s16vector_set(v1, 0, s0);
    pmt::s16vector_set(v1, 1, s1);
    pmt::s16vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::s16vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::s16vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::s16vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::s16vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::s16vector_set(v1, N, int16_t(0)), pmt::out_of_range);

    size_t len;
    const int16_t* rd = pmt::s16vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    int16_t* wr = pmt::s16vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = int16_t(0);
    BOOST_CHECK_EQUAL(int16_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_u32vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_u32vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    uint32_t s0 = uint32_t(10);
    uint32_t s1 = uint32_t(20);
    uint32_t s2 = uint32_t(30);

    pmt::u32vector_set(v1, 0, s0);
    pmt::u32vector_set(v1, 1, s1);
    pmt::u32vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::u32vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::u32vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::u32vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::u32vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::u32vector_set(v1, N, uint32_t(0)), pmt::out_of_range);

    size_t len;
    const uint32_t* rd = pmt::u32vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    uint32_t* wr = pmt::u32vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = uint32_t(0);
    BOOST_CHECK_EQUAL(uint32_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_s32vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_s32vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    int32_t s0 = int32_t(10);
    int32_t s1 = int32_t(20);
    int32_t s2 = int32_t(30);

    pmt::s32vector_set(v1, 0, s0);
    pmt::s32vector_set(v1, 1, s1);
    pmt::s32vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::s32vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::s32vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::s32vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::s32vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::s32vector_set(v1, N, int32_t(0)), pmt::out_of_range);

    size_t len;
    const int32_t* rd = pmt::s32vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    int32_t* wr = pmt::s32vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = int32_t(0);
    BOOST_CHECK_EQUAL(int32_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_u64vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_u64vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    uint64_t s0 = uint64_t(10);
    uint64_t s1 = uint64_t(20);
    uint64_t s2 = uint64_t(30);

    pmt::u64vector_set(v1, 0, s0);
    pmt::u64vector_set(v1, 1, s1);
    pmt::u64vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::u64vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::u64vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::u64vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::u64vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::u64vector_set(v1, N, uint64_t(0)), pmt::out_of_range);

    size_t len;
    const uint64_t* rd = pmt::u64vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    uint64_t* wr = pmt::u64vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = uint64_t(0);
    BOOST_CHECK_EQUAL(uint64_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_s64vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_s64vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    int64_t s0 = int64_t(10);
    int64_t s1 = int64_t(20);
    int64_t s2 = int64_t(30);

    pmt::s64vector_set(v1, 0, s0);
    pmt::s64vector_set(v1, 1, s1);
    pmt::s64vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::s64vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::s64vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::s64vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::s64vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::s64vector_set(v1, N, int64_t(0)), pmt::out_of_range);

    size_t len;
    const int64_t* rd = pmt::s64vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    int64_t* wr = pmt::s64vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = int64_t(0);
    BOOST_CHECK_EQUAL(int64_t(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_f32vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_f32vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    float s0 = float(10);
    float s1 = float(20);
    float s2 = float(30);

    pmt::f32vector_set(v1, 0, s0);
    pmt::f32vector_set(v1, 1, s1);
    pmt::f32vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::f32vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::f32vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::f32vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::f32vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::f32vector_set(v1, N, float(0)), pmt::out_of_range);

    size_t len;
    const float* rd = pmt::f32vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    float* wr = pmt::f32vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = float(0);
    BOOST_CHECK_EQUAL(float(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);

    BOOST_CHECK_EQUAL(pmt::write_string(v1), "#[0.000000 20.000000 30.000000]");
}
BOOST_AUTO_TEST_CASE(test_f64vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_f64vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    double s0 = double(10);
    double s1 = double(20);
    double s2 = double(30);

    pmt::f64vector_set(v1, 0, s0);
    pmt::f64vector_set(v1, 1, s1);
    pmt::f64vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::f64vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::f64vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::f64vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::f64vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::f64vector_set(v1, N, double(0)), pmt::out_of_range);

    size_t len;
    const double* rd = pmt::f64vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    double* wr = pmt::f64vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = double(0);
    BOOST_CHECK_EQUAL(double(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);

    BOOST_CHECK_EQUAL(pmt::write_string(v1),
                      "#[0.000000000000000 20.000000000000000 30.000000000000000]");
}
BOOST_AUTO_TEST_CASE(test_c32vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_c32vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    std::complex<float> s0 = std::complex<float>(10);
    std::complex<float> s1 = std::complex<float>(20);
    std::complex<float> s2 = std::complex<float>(30);

    pmt::c32vector_set(v1, 0, s0);
    pmt::c32vector_set(v1, 1, s1);
    pmt::c32vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::c32vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::c32vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::c32vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::c32vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::c32vector_set(v1, N, std::complex<float>(0)),
                      pmt::out_of_range);

    size_t len;
    const std::complex<float>* rd = pmt::c32vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    std::complex<float>* wr = pmt::c32vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = std::complex<float>(0);
    BOOST_CHECK_EQUAL(std::complex<float>(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
BOOST_AUTO_TEST_CASE(test_c64vector)
{
    static const size_t N = 3;
    pmt::pmt_t v1 = pmt::make_c64vector(N, 0);
    BOOST_CHECK_EQUAL(N, pmt::length(v1));
    std::complex<double> s0 = std::complex<double>(10);
    std::complex<double> s1 = std::complex<double>(20);
    std::complex<double> s2 = std::complex<double>(30);

    pmt::c64vector_set(v1, 0, s0);
    pmt::c64vector_set(v1, 1, s1);
    pmt::c64vector_set(v1, 2, s2);

    BOOST_CHECK_EQUAL(s0, pmt::c64vector_ref(v1, 0));
    BOOST_CHECK_EQUAL(s1, pmt::c64vector_ref(v1, 1));
    BOOST_CHECK_EQUAL(s2, pmt::c64vector_ref(v1, 2));

    BOOST_CHECK_THROW(pmt::c64vector_ref(v1, N), pmt::out_of_range);
    BOOST_CHECK_THROW(pmt::c64vector_set(v1, N, std::complex<double>(0)),
                      pmt::out_of_range);

    size_t len;
    const std::complex<double>* rd = pmt::c64vector_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    BOOST_CHECK_EQUAL(s0, rd[0]);
    BOOST_CHECK_EQUAL(s1, rd[1]);
    BOOST_CHECK_EQUAL(s2, rd[2]);

    std::complex<double>* wr = pmt::c64vector_writable_elements(v1, len);
    BOOST_CHECK_EQUAL(len, N);
    wr[0] = std::complex<double>(0);
    BOOST_CHECK_EQUAL(std::complex<double>(0), wr[0]);
    BOOST_CHECK_EQUAL(s1, wr[1]);
    BOOST_CHECK_EQUAL(s2, wr[2]);
}
