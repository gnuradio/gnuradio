#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_8sc_multiply_conjugate_16sc_aligned16.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>

#define assertcomplexEqual(expected, actual, delta)			\
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::real(expected), std::real(actual), fabs(std::real(expected)) * delta); \
  CPPUNIT_ASSERT_DOUBLES_EQUAL (std::imag(expected), std::imag(actual), fabs(std::imag(expected))* delta);	

#define	ERR_DELTA	(1e-4)

#ifndef LV_HAVE_SSE4_1

void qa_8sc_multiply_conjugate_16sc_aligned16::t1() {
  printf("sse4.1 not available... no test performed\n");
}

#else

void qa_8sc_multiply_conjugate_16sc_aligned16::t1() {
  
  
  volk_runtime_init();

  const int vlen = 2046;
  const int ITERS = 100000;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<int8_t>* input;
  std::complex<int8_t>* taps;
  
  std::complex<int16_t>* result_generic;
  std::complex<int16_t>* result_sse4_1;
  int i;
  int8_t* inputInt8_T;
  int8_t* tapsInt8_T;

  ret = posix_memalign((void**)&input, 16, vlen*2*sizeof(int8_t));
  ret = posix_memalign((void**)&taps, 16, vlen*2*sizeof(int8_t));
  ret = posix_memalign((void**)&result_generic, 16, vlen*2*sizeof(int16_t));
  ret = posix_memalign((void**)&result_sse4_1, 16, vlen*2*sizeof(int16_t));
  
  inputInt8_T = (int8_t*)input;
  tapsInt8_T = (int8_t*)taps;
  for(int i = 0; i < vlen*2; ++i) {   
    inputInt8_T[i] =((int8_t)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 128.0));
    tapsInt8_T[i] =((int8_t)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 128.0));
  }
  
  printf("8sc_multiply_conjugate_16sc_aligned16\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_8sc_multiply_conjugate_16sc_aligned16_manual((std::complex<int16_t>*)result_generic, (std::complex<int8_t>*)input, (std::complex<int8_t>*)taps, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);


  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_8sc_multiply_conjugate_16sc_aligned16((std::complex<int16_t>*)result_sse4_1, (std::complex<int8_t>*)input, (std::complex<int8_t>*)taps, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4_1_time: %f\n", total);

  for(i = 0; i < vlen; i++){
    //printf("%d %d+%di %d+%di -> %d+%di %d+%di\n", i, std::real(input[i]), std::imag(input[i]), std::real(taps[i]), std::imag(taps[i]), std::real(result_generic[i]), std::imag(result_generic[i]), std::real(result_sse4_1[i]), std::imag(result_sse4_1[i]));

    assertcomplexEqual(result_generic[i], result_sse4_1[i], ERR_DELTA);
  }

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse4_1);
  
}

#endif /*LV_HAVE_SSE4_1*/
