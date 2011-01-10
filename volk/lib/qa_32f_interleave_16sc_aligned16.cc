#include <volk/volk.h>
#include <qa_32f_interleave_16sc_aligned16.h>
#include <volk/volk_32f_interleave_16sc_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSE2

void qa_32f_interleave_16sc_aligned16::t1() {
  printf("sse2 not available... no test performed\n");
}

#else

void qa_32f_interleave_16sc_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  float input0[vlen] __attribute__ ((aligned (16)));
  float input1[vlen] __attribute__ ((aligned (16)));
  
  std::complex<int16_t> output_generic[vlen] __attribute__ ((aligned (16)));
  std::complex<int16_t> output_sse[vlen] __attribute__ ((aligned (16)));
  std::complex<int16_t> output_sse2[vlen] __attribute__ ((aligned (16)));

  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2)); 
    input1[i] = ((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2)); 
  }
  printf("32f_interleave_16sc_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_interleave_16sc_aligned16_manual(output_generic, input0, input1, 32768.0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_interleave_16sc_aligned16_manual(output_sse, input0, input1, 32768.0, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_32f_interleave_16sc_aligned16_manual(output_sse2, input0, input1, 32768.0, vlen, "sse2");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse2_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%d...%d\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(std::real(output_generic[i]), std::real(output_sse[i]), 1.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(std::imag(output_generic[i]), std::imag(output_sse[i]), 1.01);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(std::real(output_generic[i]), std::real(output_sse2[i]), 1.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(std::imag(output_generic[i]), std::imag(output_sse2[i]), 1.01);
  }
}

#endif
