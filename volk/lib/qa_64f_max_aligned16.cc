#include <volk/volk.h>
#include <qa_64f_max_aligned16.h>
#include <volk/volk_64f_max_aligned16.h>
#include <cstdlib>

//test for sse

#ifndef LV_HAVE_SSE2

void qa_64f_max_aligned16::t1() {
  printf("sse2 not available... no test performed\n");
}

#else

void qa_64f_max_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  double input0[vlen] __attribute__ ((aligned (16)));
  double input1[vlen] __attribute__ ((aligned (16)));
  
  double output0[vlen] __attribute__ ((aligned (16)));
  double output01[vlen] __attribute__ ((aligned (16)));

  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((double) (rand() - (RAND_MAX/2))) / static_cast<double>((RAND_MAX/2));
    input1[i] = ((double) (rand() - (RAND_MAX/2))) / static_cast<double>((RAND_MAX/2));
  }
  printf("64f_max_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_64f_max_aligned16_manual(output0, input0, input1, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_64f_max_aligned16_manual(output01, input0, input1, vlen, "sse2");
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
  }
}

#endif
