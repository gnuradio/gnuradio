#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_16s_max_star_horizontal_aligned16.h>
#include <volk/volk_16s_max_star_horizontal_aligned16.h>
#include <cstdlib>
#include <time.h>
//test for ssse3

#ifndef LV_HAVE_SSSE3

void qa_16s_max_star_horizontal_aligned16::t1() {
  printf("ssse3 not available... no test performed\n");
}

#else


void qa_16s_max_star_horizontal_aligned16::t1() {

  
  volk_runtime_init();
  
  volk_environment_init();
  clock_t start, end;
  double total;
  const int vlen = 32;
  const int ITERS = 1;
  short input0[vlen] __attribute__ ((aligned (16)));
  short output0[vlen>>1] __attribute__ ((aligned (16)));

  short output1[vlen>>1] __attribute__ ((aligned (16)));

  for(int i = 0; i < vlen; ++i) {
    short plus0 = ((short) (rand() - (RAND_MAX/2)));
    
    short minus0 = ((short) (rand() - (RAND_MAX/2)));
    
    input0[i] = plus0 - minus0;
    
  }
  printf("16s_max_star_horizontal_aligned\n");
  
  start = clock();
  for(int count = 0; count < ITERS; ++count) {
    volk_16s_max_star_horizontal_aligned16_manual(output0, input0, 2*vlen, "generic");
    volk_16s_max_star_horizontal_aligned16_manual(output0, output0, vlen, "generic");
    volk_16s_max_star_horizontal_aligned16_manual(output0, output0, vlen/2, "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);
  start = clock();
  for(int count = 0; count < ITERS; ++count) {

    get_volk_runtime()->volk_16s_max_star_horizontal_aligned16(output1, input0, 2*vlen);
    get_volk_runtime()->volk_16s_max_star_horizontal_aligned16(output1, output1, vlen);
    get_volk_runtime()->volk_16s_max_star_horizontal_aligned16(output1, output1, vlen);
    /*    volk_16s_max_star_horizontal_aligned16(output1, input0, 2*vlen, "ssse3");
    volk_16s_max_star_horizontal_aligned16(output1, output1, vlen, "ssse3");
    volk_16s_max_star_horizontal_aligned16(output1, output1, vlen, "ssse3");*/
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("ssse3_time: %f\n", total);
  
  for(int i = 0; i < (vlen >> 1); ++i) {
    //    printf("inputs: %d, %d\n", input0[i*2], input0[i*2 + 1]);
    //printf("generic... %d, ssse3... %d\n", output0[i], output1[i]);
    
  }
  for(int i = 0; i < (vlen >> 1); ++i) {
      
      CPPUNIT_ASSERT_EQUAL(output0[i], output1[i]);
    }
	}
   
  
#endif
	
