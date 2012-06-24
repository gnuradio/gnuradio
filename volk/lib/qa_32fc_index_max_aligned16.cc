#include <volk/volk.h>
#include <qa_32fc_index_max_aligned16.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ERR_DELTA (1e-4)
#define NUM_ITERS 1000000
#define VEC_LEN 3096
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


#ifndef LV_HAVE_SSE3

void qa_32fc_index_max_aligned16::t1(){
  printf("sse3 not available... no test performed\n");
}

#else


void qa_32fc_index_max_aligned16::t1(){

  const int vlen = VEC_LEN;

  volk_environment_init();
  int ret;

  unsigned int* target;
  unsigned int* target_generic;
  std::complex<float>* src0 ;


  unsigned int i_target;
  target = &i_target;
  unsigned int i_target_generic;
  target_generic = &i_target_generic;
  ret = posix_memalign((void**)&src0, 16, vlen << 3);

  random_floats((float*)src0, vlen * 2);

  printf("32fc_index_max_aligned16\n");

  clock_t start, end;
  double total;


  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32fc_index_max_aligned16_manual(target_generic, src0, vlen << 3, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic time: %f\n", total);

  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
  volk_32fc_index_max_aligned16_manual(target, src0, vlen << 3, "sse3");
  }

  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3 time: %f\n", total);




  printf("generic: %u, sse3: %u\n", target_generic[0], target[0]);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(target_generic[0], target[0], 1.1);



  free(src0);
}

#endif /*LV_HAVE_SSE3*/
