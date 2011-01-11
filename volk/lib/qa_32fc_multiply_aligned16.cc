#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32fc_multiply_aligned16.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



#define assertcomplexEqual(expected, actual, delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::real(expected), std::real(actual), fabs(std::real(expected)) * delta); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::imag(expected), std::imag(actual), fabs(std::imag(expected))* delta);	

#define	ERR_DELTA	(1e-3)

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

#ifdef LV_HAVE_SSE3
void qa_32fc_multiply_aligned16::t1() {

  const int vlen = 2046;
  const int ITERS = 100000;

  int i;
  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  std::complex<float>* taps;
  
  std::complex<float>* result_generic;
  std::complex<float>* result_sse3;
  std::complex<float>* result_orc;

  ret = posix_memalign((void**)&input, 16, vlen*2*sizeof(float));
  ret = posix_memalign((void**)&taps, 16, vlen*2*sizeof(float));
  ret = posix_memalign((void**)&result_generic, 16, vlen*2*sizeof(float));
  ret = posix_memalign((void**)&result_sse3, 16, vlen*2*sizeof(float));
  ret = posix_memalign((void**)&result_orc, 16, vlen*2*sizeof(float));
  
  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);
  
  printf("32fc_multiply_aligned16\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32fc_multiply_aligned16_manual(result_generic, input, taps, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32fc_multiply_aligned16_manual(result_sse3, input, taps, vlen, "sse3");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3_time: %f\n", total);
  
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32fc_multiply_aligned16_manual(result_orc, input, taps, vlen, "orc");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("orc_time: %f\n", total);

  for(i = 0; i < vlen; i++){
    assertcomplexEqual(result_generic[i], result_sse3[i], ERR_DELTA);
    assertcomplexEqual(result_generic[i], result_orc[i], ERR_DELTA);
  }

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse3);
  free(result_orc);
  
}
#else
void qa_32fc_multiply_aligned16::t1() {
  printf("sse3 not available... no test performed\n");
}

#endif /* LV_HAVE_SSE3 */
