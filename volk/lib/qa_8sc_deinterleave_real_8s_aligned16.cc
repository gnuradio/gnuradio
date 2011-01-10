#include <volk/volk.h>
#include <qa_8sc_deinterleave_real_8s_aligned16.h>
#include <volk/volk_8sc_deinterleave_real_8s_aligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse

#ifndef LV_HAVE_SSSE3

void qa_8sc_deinterleave_real_8s_aligned16::t1() {
  printf("ssse3 not available... no test performed\n");
}

#else

void qa_8sc_deinterleave_real_8s_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  std::complex<int8_t> input0[vlen] __attribute__ ((aligned (16)));
  
  int8_t output_generic[vlen] __attribute__ ((aligned (16)));
  int8_t output_ssse3[vlen] __attribute__ ((aligned (16)));

  int8_t* loadInput = (int8_t*)input0;
  for(int i = 0; i < vlen*2; ++i) {   
    loadInput[i] =((char)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 128.0));
  }
  printf("8sc_deinterleave_real_8s_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_8sc_deinterleave_real_8s_aligned16_manual(output_generic, input0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_8sc_deinterleave_real_8s_aligned16_manual(output_ssse3, input0, vlen, "ssse3");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("ssse3_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%d...%d\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_EQUAL(output_generic[i], output_ssse3[i]);
  }
}

#endif
