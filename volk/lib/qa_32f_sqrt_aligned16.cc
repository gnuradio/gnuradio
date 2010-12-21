/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, see 
 * <http://www.gnu.org/licenses/>.
 */

#include <volk/volk.h>
#include <qa_32f_sqrt_aligned16.h>
#include <volk/volk_32f_sqrt_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32f_sqrt_aligned16::t1() {
  printf("sse not available... no test performed\n");
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 10000;
  float input0[vlen] __attribute__ ((aligned (16)));
  
  float output0[vlen] __attribute__ ((aligned (16)));
  float output_known[vlen] __attribute__ ((aligned (16)));

  // No reason to test negative numbers because they result in NaN.
  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((float) (rand()) / static_cast<float>(RAND_MAX));
    output_known[i] = sqrt(input0[i]);
  }
  printf("32f_sqrt_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_sqrt_aligned16_manual(output0, input0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  
  /*
  for(int i = 0; i < 10; ++i) {
    printf("inputs: %f\n", input0[i]);
    printf("generic... %f == %f\n", output0[i], output_known[i]);
  }
  */
  
  for(int i = 0; i < vlen; ++i) {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output0[i], output_known[i], fabs(output0[i])*1e-4);
  }
}

#else

void qa_32f_sqrt_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  float input0[vlen] __attribute__ ((aligned (16)));
  
  float output0[vlen] __attribute__ ((aligned (16)));
  float output01[vlen] __attribute__ ((aligned (16)));

  // No reason to test negative numbers because they result in NaN.
  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((float) (rand()) / static_cast<float>(RAND_MAX));
  }
  printf("32f_sqrt_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_sqrt_aligned16_manual(output0, input0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_sqrt_aligned16_manual(output01, input0, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);
  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%d...%d\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output0[i], output01[i], fabs(output0[i])*1e-4);
  }
}

#endif
