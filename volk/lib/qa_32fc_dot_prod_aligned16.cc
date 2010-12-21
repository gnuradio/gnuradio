#include <volk/volk.h>
#include <qa_32fc_dot_prod_aligned16.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>



#define assertcomplexEqual(expected, actual, delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::real(expected), std::real(actual), fabs(std::real(expected)) * delta); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::imag(expected), std::imag(actual), fabs(std::imag(expected))* delta);	

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



#if LV_HAVE_SSE3
void qa_32fc_dot_prod_aligned16::t1() {

  const int vlen = 2046;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  std::complex<float>* taps;
  
  std::complex<float>* result_generic;
  std::complex<float>* result_sse3;

  ret = posix_memalign((void**)&input, 16, vlen << 3);
  ret = posix_memalign((void**)&taps, 16, vlen << 3);
  ret = posix_memalign((void**)&result_generic, 16, 8);
  ret = posix_memalign((void**)&result_sse3, 16, 8);
  

  result_generic[0] = std::complex<float>(0,0);
  result_sse3[0] = std::complex<float>(0,0);

  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);
  
  printf("32fc_dot_prod_aligned16\n");
  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_generic, input, taps, vlen * 8,  "generic");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_sse3, input, taps, vlen * 8, "sse3");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3_time: %f\n", total);

  printf("generic: %f +i%f ... sse3: %f +i%f\n", std::real(result_generic[0]), std::imag(result_generic[0]), std::real(result_sse3[0]), std::imag(result_sse3[0]));

  
  assertcomplexEqual(result_generic[0], result_sse3[0], ERR_DELTA);

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse3);
  
}

#else
void qa_32fc_dot_prod_aligned16::t1() {
  printf("sse3 not available... no test performed\n");
}

#endif

#if LV_HAVE_SSE && LV_HAVE_32
void qa_32fc_dot_prod_aligned16::t2() {

  const int vlen = 2046;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  std::complex<float>* taps;
  
  std::complex<float>* result_generic;
  std::complex<float>* result_sse3;

  ret = posix_memalign((void**)&input, 16, vlen << 3);
  ret = posix_memalign((void**)&taps, 16, vlen << 3);
  ret = posix_memalign((void**)&result_generic, 16, 8);
  ret = posix_memalign((void**)&result_sse3, 16, 8);
  

  result_generic[0] = std::complex<float>(0,0);
  result_sse3[0] = std::complex<float>(0,0);

  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);
  
  printf("32fc_dot_prod_aligned16\n");
  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_generic, input, taps, vlen * 8,  "generic");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_sse3, input, taps, vlen * 8, "sse_32");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_32_time: %f\n", total);

  printf("generic: %f +i%f ... sse_32: %f +i%f\n", std::real(result_generic[0]), std::imag(result_generic[0]), std::real(result_sse3[0]), std::imag(result_sse3[0]));

  
  assertcomplexEqual(result_generic[0], result_sse3[0], ERR_DELTA);

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse3);
  
}

#else
void qa_32fc_dot_prod_aligned16::t2() {
  printf("sse_32 not available... no test performed\n");
}

#endif

#if LV_HAVE_SSE && LV_HAVE_64

void qa_32fc_dot_prod_aligned16::t3() {

  const int vlen = 2046;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  std::complex<float>* taps;
  
  std::complex<float>* result_generic;
  std::complex<float>* result_sse3;

  ret = posix_memalign((void**)&input, 16, vlen << 3);
  ret = posix_memalign((void**)&taps, 16, vlen << 3);
  ret = posix_memalign((void**)&result_generic, 16, 8);
  ret = posix_memalign((void**)&result_sse3, 16, 8);
  

  result_generic[0] = std::complex<float>(0,0);
  result_sse3[0] = std::complex<float>(0,0);

  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);
  
  printf("32fc_dot_prod_aligned16\n");
  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_generic, input, taps, vlen * 8,  "generic");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  
  start = clock();
  volk_32fc_dot_prod_aligned16_manual(result_sse3, input, taps, vlen * 8, "sse_64");
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_64_time: %f\n", total);

  printf("generic: %f +i%f ... sse_64: %f +i%f\n", std::real(result_generic[0]), std::imag(result_generic[0]), std::real(result_sse3[0]), std::imag(result_sse3[0]));

  
  assertcomplexEqual(result_generic[0], result_sse3[0], ERR_DELTA);

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse3);
  
}

#else
void qa_32fc_dot_prod_aligned16::t3() {
  printf("sse_64 not available... no test performed\n");
}



#endif 
