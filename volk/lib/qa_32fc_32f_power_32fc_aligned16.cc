#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32fc_32f_power_32fc_aligned16.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define assertcomplexEqual(expected, actual, delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::real(expected), std::real(actual), fabs(std::real(expected)) * delta); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::imag(expected), std::imag(actual), fabs(std::imag(expected))* delta);	

#define	ERR_DELTA	(1.5e-3)

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
void qa_32fc_32f_power_32fc_aligned16::t1() {

  const int vlen = 2046;
  const int ITERS = 10000;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  int i;
  
  std::complex<float>* result_generic;
  std::complex<float>* result_sse;

  ret = posix_memalign((void**)&input, 16, vlen * 2 * sizeof(float));
  ret = posix_memalign((void**)&result_generic, 16, vlen * 2 * sizeof(float));
  ret = posix_memalign((void**)&result_sse, 16, vlen * 2 * sizeof(float));

  random_floats((float*)input, vlen * 2);

  const float power = 3.2;
  
  printf("32fc_32f_power_32fc_aligned16\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32fc_32f_power_32fc_aligned16_manual(result_generic, input, power, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32fc_32f_power_32fc_aligned16_manual(result_sse, input, power, vlen,  "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  for(i = 0; i < vlen; i++){
    assertcomplexEqual(result_generic[i], result_sse[i], ERR_DELTA);
  }

  free(input);
  free(result_generic);
  free(result_sse);
  
}
#else
void qa_32fc_32f_power_32fc_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#endif /* LV_HAVE_SSE */

