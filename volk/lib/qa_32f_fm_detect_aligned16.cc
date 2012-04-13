#include <volk/volk.h>
#include <qa_32f_fm_detect_aligned16.h>
#include <volk/volk_32f_fm_detect_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32f_fm_detect_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#else

void qa_32f_fm_detect_aligned16::t1() {

  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 10000;
  __VOLK_ATTR_ALIGNED(16) float input0[vlen];

  __VOLK_ATTR_ALIGNED(16) float output0[vlen];
  __VOLK_ATTR_ALIGNED(16) float output01[vlen];

  for(int i = 0; i < vlen; ++i) {
    input0[i] = ((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2));
  }
  printf("32f_fm_detect_aligned\n");

  start = clock();
  float save = 0.1;
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_fm_detect_aligned16_manual(output0, input0, 1.0, &save, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  save = 0.1;
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_fm_detect_aligned16_manual(output01, input0, 1.0, &save, vlen, "sse");
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
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output0[i], output01[i], fabs(output0[i]) * 1e-4);
  }
}

#endif
