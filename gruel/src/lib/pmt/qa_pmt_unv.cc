/* -*- c++ -*- */
/*
 * Copyright 2006,2009 Free Software Foundation, Inc.
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

#include <qa_pmt_unv.h>
#include <cppunit/TestAssert.h>
#include <gruel/pmt.h>
#include <stdio.h>

using namespace pmt;
void
qa_pmt_unv::test_u8vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_u8vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  uint8_t s0 = uint8_t(10);
  uint8_t s1 = uint8_t(20);
  uint8_t s2 = uint8_t(30);

  pmt_u8vector_set(v1, 0, s0);
  pmt_u8vector_set(v1, 1, s1);
  pmt_u8vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_u8vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_u8vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_u8vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_u8vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_u8vector_set(v1, N, uint8_t(0)), pmt_out_of_range);

  size_t	len;
  const uint8_t *rd = pmt_u8vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  uint8_t *wr = pmt_u8vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = uint8_t(0);
  CPPUNIT_ASSERT_EQUAL(uint8_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_s8vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_s8vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  int8_t s0 = int8_t(10);
  int8_t s1 = int8_t(20);
  int8_t s2 = int8_t(30);

  pmt_s8vector_set(v1, 0, s0);
  pmt_s8vector_set(v1, 1, s1);
  pmt_s8vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_s8vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_s8vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_s8vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_s8vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_s8vector_set(v1, N, int8_t(0)), pmt_out_of_range);

  size_t	len;
  const int8_t *rd = pmt_s8vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  int8_t *wr = pmt_s8vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = int8_t(0);
  CPPUNIT_ASSERT_EQUAL(int8_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_u16vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_u16vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  uint16_t s0 = uint16_t(10);
  uint16_t s1 = uint16_t(20);
  uint16_t s2 = uint16_t(30);

  pmt_u16vector_set(v1, 0, s0);
  pmt_u16vector_set(v1, 1, s1);
  pmt_u16vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_u16vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_u16vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_u16vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_u16vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_u16vector_set(v1, N, uint16_t(0)), pmt_out_of_range);

  size_t	len;
  const uint16_t *rd = pmt_u16vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  uint16_t *wr = pmt_u16vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = uint16_t(0);
  CPPUNIT_ASSERT_EQUAL(uint16_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_s16vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_s16vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  int16_t s0 = int16_t(10);
  int16_t s1 = int16_t(20);
  int16_t s2 = int16_t(30);

  pmt_s16vector_set(v1, 0, s0);
  pmt_s16vector_set(v1, 1, s1);
  pmt_s16vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_s16vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_s16vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_s16vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_s16vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_s16vector_set(v1, N, int16_t(0)), pmt_out_of_range);

  size_t	len;
  const int16_t *rd = pmt_s16vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  int16_t *wr = pmt_s16vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = int16_t(0);
  CPPUNIT_ASSERT_EQUAL(int16_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_u32vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_u32vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  uint32_t s0 = uint32_t(10);
  uint32_t s1 = uint32_t(20);
  uint32_t s2 = uint32_t(30);

  pmt_u32vector_set(v1, 0, s0);
  pmt_u32vector_set(v1, 1, s1);
  pmt_u32vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_u32vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_u32vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_u32vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_u32vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_u32vector_set(v1, N, uint32_t(0)), pmt_out_of_range);

  size_t	len;
  const uint32_t *rd = pmt_u32vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  uint32_t *wr = pmt_u32vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = uint32_t(0);
  CPPUNIT_ASSERT_EQUAL(uint32_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_s32vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_s32vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  int32_t s0 = int32_t(10);
  int32_t s1 = int32_t(20);
  int32_t s2 = int32_t(30);

  pmt_s32vector_set(v1, 0, s0);
  pmt_s32vector_set(v1, 1, s1);
  pmt_s32vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_s32vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_s32vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_s32vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_s32vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_s32vector_set(v1, N, int32_t(0)), pmt_out_of_range);

  size_t	len;
  const int32_t *rd = pmt_s32vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  int32_t *wr = pmt_s32vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = int32_t(0);
  CPPUNIT_ASSERT_EQUAL(int32_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_u64vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_u64vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  uint64_t s0 = uint64_t(10);
  uint64_t s1 = uint64_t(20);
  uint64_t s2 = uint64_t(30);

  pmt_u64vector_set(v1, 0, s0);
  pmt_u64vector_set(v1, 1, s1);
  pmt_u64vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_u64vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_u64vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_u64vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_u64vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_u64vector_set(v1, N, uint64_t(0)), pmt_out_of_range);

  size_t	len;
  const uint64_t *rd = pmt_u64vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  uint64_t *wr = pmt_u64vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = uint64_t(0);
  CPPUNIT_ASSERT_EQUAL(uint64_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_s64vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_s64vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  int64_t s0 = int64_t(10);
  int64_t s1 = int64_t(20);
  int64_t s2 = int64_t(30);

  pmt_s64vector_set(v1, 0, s0);
  pmt_s64vector_set(v1, 1, s1);
  pmt_s64vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_s64vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_s64vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_s64vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_s64vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_s64vector_set(v1, N, int64_t(0)), pmt_out_of_range);

  size_t	len;
  const int64_t *rd = pmt_s64vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  int64_t *wr = pmt_s64vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = int64_t(0);
  CPPUNIT_ASSERT_EQUAL(int64_t(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_f32vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_f32vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  float s0 = float(10);
  float s1 = float(20);
  float s2 = float(30);

  pmt_f32vector_set(v1, 0, s0);
  pmt_f32vector_set(v1, 1, s1);
  pmt_f32vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_f32vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_f32vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_f32vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_f32vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_f32vector_set(v1, N, float(0)), pmt_out_of_range);

  size_t	len;
  const float *rd = pmt_f32vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  float *wr = pmt_f32vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = float(0);
  CPPUNIT_ASSERT_EQUAL(float(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_f64vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_f64vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  double s0 = double(10);
  double s1 = double(20);
  double s2 = double(30);

  pmt_f64vector_set(v1, 0, s0);
  pmt_f64vector_set(v1, 1, s1);
  pmt_f64vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_f64vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_f64vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_f64vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_f64vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_f64vector_set(v1, N, double(0)), pmt_out_of_range);

  size_t	len;
  const double *rd = pmt_f64vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  double *wr = pmt_f64vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = double(0);
  CPPUNIT_ASSERT_EQUAL(double(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_c32vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_c32vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  std::complex<float> s0 = std::complex<float>(10);
  std::complex<float> s1 = std::complex<float>(20);
  std::complex<float> s2 = std::complex<float>(30);

  pmt_c32vector_set(v1, 0, s0);
  pmt_c32vector_set(v1, 1, s1);
  pmt_c32vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_c32vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_c32vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_c32vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_c32vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_c32vector_set(v1, N, std::complex<float>(0)), pmt_out_of_range);

  size_t	len;
  const std::complex<float> *rd = pmt_c32vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  std::complex<float> *wr = pmt_c32vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = std::complex<float>(0);
  CPPUNIT_ASSERT_EQUAL(std::complex<float>(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
void
qa_pmt_unv::test_c64vector()
{
  static const size_t N = 3;
  pmt_t v1 = pmt_make_c64vector(N, 0);
  CPPUNIT_ASSERT_EQUAL(N, pmt_length(v1));
  std::complex<double> s0 = std::complex<double>(10);
  std::complex<double> s1 = std::complex<double>(20);
  std::complex<double> s2 = std::complex<double>(30);

  pmt_c64vector_set(v1, 0, s0);
  pmt_c64vector_set(v1, 1, s1);
  pmt_c64vector_set(v1, 2, s2);

  CPPUNIT_ASSERT_EQUAL(s0, pmt_c64vector_ref(v1, 0));
  CPPUNIT_ASSERT_EQUAL(s1, pmt_c64vector_ref(v1, 1));
  CPPUNIT_ASSERT_EQUAL(s2, pmt_c64vector_ref(v1, 2));

  CPPUNIT_ASSERT_THROW(pmt_c64vector_ref(v1, N), pmt_out_of_range);
  CPPUNIT_ASSERT_THROW(pmt_c64vector_set(v1, N, std::complex<double>(0)), pmt_out_of_range);

  size_t	len;
  const std::complex<double> *rd = pmt_c64vector_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  CPPUNIT_ASSERT_EQUAL(s0, rd[0]);
  CPPUNIT_ASSERT_EQUAL(s1, rd[1]);
  CPPUNIT_ASSERT_EQUAL(s2, rd[2]);

  std::complex<double> *wr = pmt_c64vector_writable_elements(v1, len);
  CPPUNIT_ASSERT_EQUAL(len, N);
  wr[0] = std::complex<double>(0);
  CPPUNIT_ASSERT_EQUAL(std::complex<double>(0), wr[0]);
  CPPUNIT_ASSERT_EQUAL(s1, wr[1]);
  CPPUNIT_ASSERT_EQUAL(s2, wr[2]);
}
