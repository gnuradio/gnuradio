#include <volk/volk.h>
#include <qa_32f_normalize_aligned16.h>
#include <volk/volk_32f_normalize_aligned16.h>
#include <cstdlib>
#include <cstring>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32f_normalize_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#else

void qa_32f_normalize_aligned16::t1() {
  
  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  const int vlen = 320001;
  const int ITERS = 100;

  float* output0;
  float* output01;
  ret = posix_memalign((void**)&output0, 16, vlen*sizeof(float));
  ret = posix_memalign((void**)&output01, 16, vlen*sizeof(float));

  for(int i = 0; i < vlen; ++i) {   
    output0[i] = ((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2));
  }
  memcpy(output01, output0, vlen*sizeof(float));
  printf("32f_normalize_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_normalize_aligned16_manual(output0, 1.15, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_normalize_aligned16_manual(output01, 1.15, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);
  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    // printf("%e...%e\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output0[i], output01[i], fabs(output0[i])*1e-4);
  }

  free(output0);
  free(output01);
}

#endif
