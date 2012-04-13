#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32f_index_max_aligned16.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERR_DELTA (1e-4)
#define NUM_ITERS 1000000
#define VEC_LEN 3097
static float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  unsigned int i = 0;
  for (; i < n; i++) {

    buf[i] = uniform () * 32767;

  }
}


#ifndef LV_HAVE_SSE

void qa_32f_index_max_aligned16::t1(){
  printf("sse not available... no test performed\n");
}

#else


void qa_32f_index_max_aligned16::t1(){

  const int vlen = VEC_LEN;


  volk_runtime_init();

  volk_environment_init();
  int ret;

  unsigned int* target_sse4_1;
  unsigned int* target_sse;
  unsigned int* target_generic;
  float* src0 ;


  unsigned int i_target_sse4_1;
  target_sse4_1 = &i_target_sse4_1;
  unsigned int i_target_sse;
  target_sse = &i_target_sse;
  unsigned int i_target_generic;
  target_generic = &i_target_generic;

  ret = posix_memalign((void**)&src0, 16, vlen *sizeof(float));

  random_floats((float*)src0, vlen);

  printf("32f_index_max_aligned16\n");

  clock_t start, end;
  double total;


  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32f_index_max_aligned16_manual(target_generic, src0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic time: %f\n", total);

  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32f_index_max_aligned16_manual(target_sse, src0, vlen, "sse2");
  }

  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse time: %f\n", total);

  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    get_volk_runtime()->volk_32f_index_max_aligned16(target_sse4_1, src0, vlen);
  }

  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4.1 time: %f\n", total);


  printf("generic: %u, sse: %u, sse4.1: %u\n", target_generic[0], target_sse[0], target_sse4_1[0]);
  CPPUNIT_ASSERT_EQUAL(target_generic[0], target_sse[0]);
  CPPUNIT_ASSERT_EQUAL(target_generic[0], target_sse4_1[0]);

  free(src0);
}

#endif /*LV_HAVE_SSE3*/
