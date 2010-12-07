#include <volk/volk.h>
#include <qa_32fc_square_dist_scalar_mult_aligned16.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ERR_DELTA .0001
#define NUM_ITERS 10000000
#define VEC_LEN 64

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

void qa_32fc_square_dist_scalar_mult_aligned16::t1(){
  printf("sse3 not available... no test performed\n");
}

#else


void qa_32fc_square_dist_scalar_mult_aligned16::t1(){
  int i = 0;
  
  const int vlen = VEC_LEN;
  
  volk_environment_init();
  int ret;
  
  float* target;
  float* target_generic;
  std::complex<float>* src0 ;
  std::complex<float>* points;
  float scalar;

  ret = posix_memalign((void**)&points, 16, vlen << 3);
  ret = posix_memalign((void**)&target, 16, vlen << 2);
  ret = posix_memalign((void**)&target_generic, 16, vlen << 2);
  ret = posix_memalign((void**)&src0, 16, 8);
  
  random_floats((float*)points, vlen * 2);
  random_floats((float*)src0, 2);
  random_floats(&scalar, 1);
  
  printf("32fc_square_dist_scalar_mult_aligned16\n");
  
  clock_t start, end;
  double total;
  
  
  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32fc_square_dist_scalar_mult_aligned16_manual(target_generic, src0, points, scalar, vlen << 3, "generic");
  }
  end = clock();  
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic time: %f\n", total);
  
  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32fc_square_dist_scalar_mult_aligned16_manual(target, src0, points, scalar, vlen << 3, "sse3");
  }
  
  end = clock();  
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3 time: %f\n", total);

  
  
  for(i = 0; i < vlen; ++i) {
    printf("generic: %f, sse3: %f\n", target_generic[i], target[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(target[i], target_generic[i], fabs(target_generic[1]) * ERR_DELTA);//, target_generic[1] * ERR_DELTA);
  }

  free(target);
  free(target_generic);
  free(points);
  free(src0);
}

#endif /*LV_HAVE_SSE3*/
