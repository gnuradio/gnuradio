#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_16sc_deinterleave_real_32f_aligned16.h>
#include <volk/volk_16sc_deinterleave_real_32f_aligned16.h>
#include <cstdlib>

//test for sse

#ifndef LV_HAVE_SSE4_1

#ifndef LV_HAVE_SSE

void qa_16sc_deinterleave_real_32f_aligned16::t1() {
  printf("sse not available... no test performed\n");
}

#else

void qa_16sc_deinterleave_real_32f_aligned16::t1() {
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  std::complex<int16_t> input0[vlen] __attribute__ ((aligned (16)));
  
  float output_generic[vlen] __attribute__ ((aligned (16)));
  float output_sse[vlen] __attribute__ ((aligned (16)));

  int16_t* loadInput = (int16_t*)input0;
  for(int i = 0; i < vlen*2; ++i) {   
    loadInput[i] =((int16_t)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 32768.0));
  }
  printf("16sc_deinterleave_real_32f_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_16sc_deinterleave_real_32f_aligned16_manual(output_generic, input0, 32768.0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_16sc_deinterleave_real_32f_aligned16_manual(output_sse, input0, 32768.0, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%d...%d\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output_generic[i], output_sse[i], fabs(output_generic[i])*1e-4);
  }
}

#endif /* SSE */

#else

void qa_16sc_deinterleave_real_32f_aligned16::t1() {
  
  volk_runtime_init();

  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  std::complex<int16_t> input0[vlen] __attribute__ ((aligned (16)));
  
  float output_generic[vlen] __attribute__ ((aligned (16)));
  float output_sse[vlen] __attribute__ ((aligned (16)));
  float output_sse4_1[vlen] __attribute__ ((aligned (16)));

  int16_t* loadInput = (int16_t*)input0;
  for(int i = 0; i < vlen*2; ++i) {   
    loadInput[i] =((int16_t)(((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 32768.0);
  }
  printf("16sc_deinterleave_real_32f_aligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_16sc_deinterleave_real_32f_aligned16_manual(output_generic, input0, 32768.0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_16sc_deinterleave_real_32f_aligned16_manual(output_sse, input0, 32768.0, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_16sc_deinterleave_real_32f_aligned16(output_sse4_1, input0, 32768.0, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4_1_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%d...%d\n", output0[i], output01[i]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output_generic[i], output_sse[i], fabs(output_generic[i])*1e-4);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(output_generic[i], output_sse4_1[i], fabs(output_generic[i])*1e-4);
  }
}

#endif /* SSE4_1 */
