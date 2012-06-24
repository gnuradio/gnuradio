#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_64u_popcnt_aligned16.h>
#include <volk/volk_64u_popcnt_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSE4_2

void qa_64u_popcnt_aligned16::t1() {
  printf("sse4.2 not available... no test performed\n");
}

#else

void qa_64u_popcnt_aligned16::t1() {


  volk_runtime_init();

  volk_environment_init();
  clock_t start, end;
  double total;

  const int ITERS = 10000000;
  __VOLK_ATTR_ALIGNED(16) uint64_t input0;

  __VOLK_ATTR_ALIGNED(16) uint64_t output0;
  __VOLK_ATTR_ALIGNED(16) uint64_t output01;

    input0 = ((uint64_t) (rand() - (RAND_MAX/2)));
    output0 = 0;
    output01 = 0;

  printf("64u_popcnt_aligned\n");

  start = clock();
  uint64_t ret = 0;
  for(int count = 0; count < ITERS; ++count) {
    volk_64u_popcnt_aligned16_manual(&ret, input0, "generic");
    output0 += ret;
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  ret = 0;
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_64u_popcnt_aligned16(&ret, input0);
    output01 += ret;
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4.2_time: %f\n", total);


  CPPUNIT_ASSERT_EQUAL(output0, output01);
}

#endif
