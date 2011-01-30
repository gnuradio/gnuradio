#include <volk/volk.h>
#include <qa_32f_sum_of_poly_aligned16.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SNR 30.0
#define CENTER -4.0
#define CUTOFF -5.595
#define ERR_DELTA (1e-4)
#define NUM_ITERS 100000
#define VEC_LEN 64
static float uniform() {
  return ((float) rand() / RAND_MAX);	// uniformly (0, 1)
}

static void
random_floats (float *buf, unsigned n)
{
  unsigned int i = 0;
  for (; i < n; i++) {

    buf[i] =  uniform () * -SNR/2.0;

  }
}


#ifndef LV_HAVE_SSE3

void qa_32f_sum_of_poly_aligned16::t1(){
  printf("sse3 not available... no test performed\n");
}

#else


void qa_32f_sum_of_poly_aligned16::t1(){
  int i = 0;
  
  volk_environment_init();
  int ret;

  const int vlen = VEC_LEN;
  float cutoff = CUTOFF;
  
  float* center_point_array;
  float* target;
  float* target_generic;
  float* src0 ;


  ret = posix_memalign((void**)&center_point_array, 16, 24);
  ret = posix_memalign((void**)&target, 16, 4);
  ret = posix_memalign((void**)&target_generic, 16, 4);
  ret = posix_memalign((void**)&src0, 16, (vlen << 2));
  
 
  random_floats((float*)src0, vlen);
 
  float a = (float)CENTER;
  float etoa = expf(a);
  center_point_array[0] = (//(5.0 * a * a * a * a)/120.0 +
			   (-4.0 * a * a * a)/24.0 + 
			   (3.0 * a * a)/6.0 +
			   (-2.0 * a)/2.0 +
			   (1.0)) * etoa;
  center_point_array[1] = (//(-10.0 * a * a * a)/120.0 +
			   (6.0 * a * a)/24.0 + 
			   (-3.0 * a)/6.0 +
			   (1.0/2.0)) * etoa;
  center_point_array[2] = (//(10.0 * a * a)/120.0 +
			   (-4.0 * a)/24.0 +
			   (1.0/6.0)) * etoa;
  center_point_array[3] = (//(-5.0 * a)/120.0 +
			   (1.0/24.0)) * etoa;
  //center_point_array[4] = ((1.0)/120.0) * etoa;
  center_point_array[4] = (//(a * a * a * a * a)/120.0 +
			   (a * a * a * a)/24.0 +
			   (a * a * a)/-6.0 +
			   (a * a)/2.0 +
			   -a + 1.0) * etoa;
  
  printf("32f_sum_of_poly_aligned16\n");

  clock_t start, end;
  double total;
  
  float my_sum = 0.0;
  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    float sum = 0.0;
    for(int l = 0; l < vlen; ++l) {
      
      sum += expf(src0[l]);
      
    }
    my_sum = sum;
  }
  
  
  end = clock();  
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("exp time: %f\n", total);
  
  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    
    volk_32f_sum_of_poly_aligned16_manual(target_generic, src0, center_point_array, &cutoff, vlen << 2, "generic");
  
  }
  
  
  end = clock();  
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("generic time: %f\n", total);
  
  start = clock();
  for(int k = 0; k < NUM_ITERS; ++k) {
    volk_32f_sum_of_poly_aligned16_manual(target, src0, center_point_array, &cutoff, vlen << 2, "sse3");
  }
  
  end = clock();  
  total = (double)(end-start)/(double)CLOCKS_PER_SEC;
  printf("sse3 approx time: %f\n", total);


  
  printf("exp: %f, sse3: %f\n", my_sum, target[i]);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(target_generic[0], target[0], fabs(target_generic[0]) * ERR_DELTA);
  

  free(center_point_array);
  free(target);
  free(target_generic);
  free(src0);

  
}

#endif /*LV_HAVE_SSE3*/
