#include <volk/volk.h>
#include <qa_32s_or_aligned16.h>
#include <volk/volk_32s_or_aligned16.h>
#include <cstdlib>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32s_or_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#else

void qa_32s_or_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  int32_t input0[vlen] __attribute__ ((aligned (16)));
  int32_t input1[vlen] __attribute__ ((aligned (16)));
  
  int32_t output0[vlen] __attribute__ ((aligned (16)));
  int32_t output01[vlen] __attribute__ ((aligned (16)));
  int32_t output02[vlen] __attribute__ ((aligned (16)));

  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((int32_t) (rand() - (RAND_MAX/2)));
    input1[i] = ((int32_t) (rand() - (RAND_MAX/2)));
  }
  printf("32s_or_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32s_or_aligned16_manual(output0, input0, input1, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32s_or_aligned16_manual(output02, input0, input1, vlen, "orc");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("orc_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32s_or_aligned16_manual(output01, input0, input1, vlen, "sse");
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
    CPPUNIT_ASSERT_EQUAL(output0[i], output01[i]);
    CPPUNIT_ASSERT_EQUAL(output0[i], output02[i]);
  }
}

#endif
