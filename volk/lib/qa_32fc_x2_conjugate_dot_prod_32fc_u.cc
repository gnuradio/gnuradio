#include <volk/volk.h>
#include <qa_32fc_x2_conjugate_dot_prod_32fc_u.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define assertcomplexEqual(expected, actual, delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::real(expected), std::real(actual), fabs(std::real(expected)) * delta); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::imag(expected), std::imag(actual), fabs(std::imag(expected))* delta);

#define	ERR_DELTA	(1e-4)

//test for sse

#if LV_HAVE_SSE && LV_HAVE_64

static float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform () * 32767;
}


void qa_32fc_x2_conjugate_dot_prod_32fc_u::t1() {
  const int vlen = 789743;

  volk_environment_init();
  int ret;

  std::complex<float>* input;
  std::complex<float>* taps;

  std::complex<float>* result_generic;
  std::complex<float>* result;

  ret = posix_memalign((void**)&input, 16, vlen << 3);
  ret = posix_memalign((void**)&taps, 16, vlen << 3);
  ret = posix_memalign((void**)&result_generic, 16, 8);
  ret = posix_memalign((void**)&result, 16, 8);


  result_generic[0] = std::complex<float>(0,0);
  result[0] = std::complex<float>(0,0);

  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);



  volk_32fc_x2_conjugate_dot_prod_32fc_u_manual(result_generic, input, taps, vlen * 8,  "generic");


  volk_32fc_x2_conjugate_dot_prod_32fc_u_manual(result, input, taps, vlen * 8, "sse");

  printf("32fc_x2_conjugate_dot_prod_32fc_u\n");
  printf("generic: %f +i%f ... sse: %f +i%f\n", std::real(result_generic[0]), std::imag(result_generic[0]), std::real(result[0]), std::imag(result[0]));

  assertcomplexEqual(result_generic[0], result[0], ERR_DELTA);

  free(input);
  free(taps);
  free(result_generic);
  free(result);

}


#elif LV_HAVE_SSE && LV_HAVE_32

static float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform () * 32767;
}


void qa_32fc_x2_conjugate_dot_prod_32fc_u::t1() {
  const int vlen = 789743;

  volk_environment_init();
  int ret;

  std::complex<float>* input;
  std::complex<float>* taps;

  std::complex<float>* result_generic;
  std::complex<float>* result;

  ret = posix_memalign((void**)&input, 16, vlen << 3);
  ret = posix_memalign((void**)&taps, 16, vlen << 3);
  ret = posix_memalign((void**)&result_generic, 16, 8);
  ret = posix_memalign((void**)&result, 16, 8);


  result_generic[0] = std::complex<float>(0,0);
  result[0] = std::complex<float>(0,0);

  random_floats((float*)input, vlen * 2);
  random_floats((float*)taps, vlen * 2);



  volk_32fc_x2_conjugate_dot_prod_32fc_u_manual(result_generic, input, taps, vlen * 8,  "generic");


  volk_32fc_x2_conjugate_dot_prod_32fc_u_manual(result, input, taps, vlen * 8, "sse_32");

  printf("32fc_x2_conjugate_dot_prod_32fc_u\n");
  printf("generic: %f +i%f ... sse: %f +i%f\n", std::real(result_generic[0]), std::imag(result_generic[0]), std::real(result[0]), std::imag(result[0]));

  assertcomplexEqual(result_generic[0], result[0], ERR_DELTA);

  free(input);
  free(taps);
  free(result_generic);
  free(result);

}


#else

void qa_32fc_x2_conjugate_dot_prod_32fc_u::t1() {
  printf("sse not available... no test performed\n");
}

#endif /*LV_HAVE_SSE*/
