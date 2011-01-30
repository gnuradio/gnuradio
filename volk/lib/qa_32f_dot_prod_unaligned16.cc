#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32f_dot_prod_unaligned16.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

#ifndef LV_HAVE_SSE4_1

#ifdef LV_HAVE_SSE3
void qa_32f_dot_prod_unaligned16::t1() {
  
  
  volk_runtime_init();

  const int vlen = 2046;
  const int ITER = 100000;

  int i;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  float * input;
  float * taps;
  
  float * result_generic;
  float * result_sse;
  float * result_sse3;

  ret = posix_memalign((void**)&input, 16, vlen* sizeof(float));
  ret = posix_memalign((void**)&taps, 16, vlen *sizeof(float));
  ret = posix_memalign((void**)&result_generic, 16, ITER*sizeof(float));
  ret = posix_memalign((void**)&result_sse, 16, ITER*sizeof(float));
  ret = posix_memalign((void**)&result_sse3, 16, ITER*sizeof(float));

  random_floats((float*)input, vlen);
  random_floats((float*)taps, vlen);
  
  
  printf("32f_dot_prod_unaligned16\n");

  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_generic[i], input, taps, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_sse[i], input, taps, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_sse3[i], input, taps, vlen, "sse3");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3_time: %f\n", total);

  printf("generic: %f ... sse: %f  ... sse3 %f \n", result_generic[0], result_sse[0], result_sse3[0]);

  for(i = 0; i < ITER; i++){
    CPPUNIT_ASSERT_DOUBLES_EQUAL (result_generic[i], result_sse[i], fabs(result_generic[i])*ERR_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (result_generic[i], result_sse3[i], fabs(result_generic[i])*ERR_DELTA);
  }

  free(input);
  free(taps);
  free(result_generic);
  free(result_sse);
  free(result_sse3);
  
}
#else
void qa_32f_dot_prod_unaligned16::t1() {
  printf("sse3 not available... no test performed\n");
}

#endif /* LV_HAVE_SSE3 */

#else

void qa_32f_dot_prod_unaligned16::t1() {
  
  
  volk_runtime_init();

  const int vlen = 4095;
  const int ITER = 100000;

  int i;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  float * input;
  float * taps;
  
  float * result_generic;
  float * result_sse;
  float * result_sse3;
  float * result_sse4_1;

  ret = posix_memalign((void**)&input, 16, (vlen+1) * sizeof(float));
  ret = posix_memalign((void**)&taps, 16, (vlen+1) * sizeof(float));
  ret = posix_memalign((void**)&result_generic, 16, ITER*sizeof(float));
  ret = posix_memalign((void**)&result_sse, 16, ITER*sizeof(float));
  ret = posix_memalign((void**)&result_sse3, 16, ITER*sizeof(float));
  ret = posix_memalign((void**)&result_sse4_1, 16, ITER*sizeof(float));

  input = &input[1]; // Make sure the buffer is unaligned
  taps = &taps[1]; // Make sure the buffer is unaligned

  random_floats((float*)input, vlen);
  random_floats((float*)taps, vlen);
  
  printf("32f_dot_prod_unaligned16\n");
  
  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_generic[i], input, taps, vlen,  "generic");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic_time: %f\n", total);

  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_sse[i], input, taps, vlen, "sse");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse_time: %f\n", total);

  start = clock();
  for(i = 0; i < ITER; i++){
    volk_32f_dot_prod_unaligned16_manual(&result_sse3[i], input, taps, vlen, "sse3");
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3_time: %f\n", total);

  start = clock();
  for(i = 0; i < ITER; i++){
    get_volk_runtime()->volk_32f_dot_prod_unaligned16(&result_sse4_1[i], input, taps, vlen);
  }
  end = clock();
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse4_1_time: %f\n", total);

  //printf("generic: %f ... sse: %f  ... sse3 %f  ... sse4_1 %f \n", result_generic[0], result_sse[0], result_sse3[0], result_sse4_1[0]);
  for(i =0; i < ITER; i++){
    CPPUNIT_ASSERT_DOUBLES_EQUAL (result_generic[i], result_sse[i], fabs(result_generic[i])*ERR_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (result_generic[i], result_sse3[i], fabs(result_generic[i])*ERR_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL (result_generic[i], result_sse4_1[i], fabs(result_generic[i])*ERR_DELTA);
  }

  free(&input[-1]);
  free(&taps[-1]);
  free(result_generic);
  free(result_sse);
  free(result_sse3);
  free(result_sse4_1);
  
}

#endif /*LV_HAVE_SSE*/
