#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32f_stddev_aligned16.h>
#include <volk/volk_32f_stddev_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSE

void qa_32f_stddev_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#else

void qa_32f_stddev_aligned16::t1() {
  volk_runtime_init();  

  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  float input0[vlen] __attribute__ ((aligned (16)));

  float stddev_generic;
  float stddev_sse;
  float stddev_sse4_1;
  float mean = 0;

  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2));
    mean += input0[i];
  }
  mean /= static_cast<float>(vlen);

  printf("32f_stddev_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_stddev_aligned16_manual(&stddev_generic, input0, mean, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_stddev_aligned16_manual(&stddev_sse, input0, mean, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_32f_stddev_aligned16(&stddev_sse4_1, input0, mean, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4_1_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  //printf("%d...%d\n", output0[i], output01[i]);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(stddev_generic, stddev_sse, fabs(stddev_generic)*1e-4);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(stddev_generic, stddev_sse4_1, fabs(stddev_generic)*1e-4);

}

#endif
