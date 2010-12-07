#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32f_power_aligned16.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define	ERR_DELTA	(1e-4)

//test for sse
static float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform ();
}

#ifdef LV_HAVE_SSE
void qa_32f_power_aligned16::t1() {

  
  volk_runtime_init();

  const int vlen = 2046;
  const int ITERS = 10000;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  float* input;
  int i;
  
  float* result_generic;
  float* result_sse;
  float* result_sse4_1;

  ret = posix_memalign((void**)&input, 16, vlen *  sizeof(float));
  ret = posix_memalign((void**)&result_generic, 16, vlen * sizeof(float));
  ret = posix_memalign((void**)&result_sse, 16, vlen * sizeof(float));
  ret = posix_memalign((void**)&result_sse4_1, 16, vlen * sizeof(float));

  random_floats((float*)input, vlen);

  const float power = 3;
  
  printf("32f_power_aligned16\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_power_aligned16_manual(result_generic, input, power, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_power_aligned16_manual(result_sse, input, power, vlen,  "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_32f_power_aligned16(result_sse4_1, input, power, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4.1_time: %f\n", total);


  for(i = 0; i < vlen; i++){
    //printf("%d %e -> %e %e %e\n", i, input[i], result_generic[i], result_sse[i], result_sse4_1[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(result_generic[i], result_sse[i], fabs(result_generic[i])* ERR_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(result_generic[i], result_sse4_1[i], fabs(result_generic[i])* ERR_DELTA);
  }

  free(input);
  free(result_generic);
  free(result_sse);
  
}
#else
void qa_32f_power_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#endif /* LV_HAVE_SSE */

