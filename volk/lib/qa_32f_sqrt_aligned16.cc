#include <volk/volk.h>
#include <qa_32f_sqrt_aligned16.h>
#include <volk/volk_32f_sqrt_aligned16.h>
#include <cstdlib>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32f_sqrt_aligned16::t1() {
  printf("sse not available... no test performed\n");
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
