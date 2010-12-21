#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_8s_convert_32f_unaligned16.h>
#include <volk/volk_8s_convert_32f_unaligned16.h>
#include <cstdlib>
#include <ctime>

//test for sse4.1

#ifndef LV_HAVE_SSE4_1

void qa_8s_convert_32f_unaligned16::t1() {
  printf("sse4_1 not available... no test performed\n");
}

#else

void qa_8s_convert_32f_unaligned16::t1() {
  
  volk_runtime_init();

  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 3201;
  const int ITERS = 100000;
  int8_t input0[vlen+1] __attribute__ ((aligned (16)));
  
  float output_generic[vlen+1] __attribute__ ((aligned (16)));
  float output_sse4_1[vlen+1] __attribute__ ((aligned (16)));

  for(int i = 0; i < vlen; ++i) {   
    input0[i] = ((int8_t)(((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2)) * 128.0));
  }
  printf("8s_convert_32f_unaligned\n");

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_8s_convert_32f_unaligned16_manual(output_generic, &input0[1], 128.0, vlen, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    get_volk_runtime()->volk_8s_convert_32f_unaligned16(output_sse4_1, &input0[1], 128.0, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4_1_time: %f\n", total);

  for(int i = 0; i < 1; ++i) {
    //printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
  }
  
  for(int i = 0; i < vlen; ++i) {
    //printf("%e...%e\n", output_generic[i], output_sse4_1[i]);
    CPPUNIT_ASSERT_EQUAL(output_generic[i], output_sse4_1[i]);
  }
}

#endif
