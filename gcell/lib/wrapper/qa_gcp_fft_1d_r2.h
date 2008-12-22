/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_QA_GCP_FFT_1D_R2_H
#define INCLUDED_QA_GCP_FFT_1D_R2_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <gcell/gc_job_manager.h>

class qa_gcp_fft_1d_r2 : public CppUnit::TestCase {

  CPPUNIT_TEST_SUITE(qa_gcp_fft_1d_r2);
  CPPUNIT_TEST(t1);
  CPPUNIT_TEST(t2);
  CPPUNIT_TEST(t3);
  CPPUNIT_TEST(t4);
  CPPUNIT_TEST_SUITE_END();

 private:
  void t1();
  void t2();
  void t3();
  void t4();

  void test(gc_job_manager_sptr mgr, int log2_fft_size, bool forward);
};



#endif /* INCLUDED_QA_GCP_FFT_1D_R2_H */
