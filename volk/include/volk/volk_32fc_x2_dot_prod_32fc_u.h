#ifndef INCLUDED_volk_32fc_x2_dot_prod_32fc_u_H
#define INCLUDED_volk_32fc_x2_dot_prod_32fc_u_H

#include <volk/volk_common.h>
#include <volk/volk_complex.h>
#include <stdio.h>
#include <string.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32fc_x2_dot_prod_32fc_u_generic(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {

  float * res = (float*) result;
  float * in = (float*) input;
  float * tp = (float*) taps;
  unsigned int n_2_ccomplex_blocks = num_points/2;
  unsigned int isodd = num_points &1;



  float sum0[2] = {0,0};
  float sum1[2] = {0,0};
  unsigned int i = 0;


  for(i = 0; i < n_2_ccomplex_blocks; ++i) {


    sum0[0] += in[0] * tp[0] - in[1] * tp[1];
    sum0[1] += in[0] * tp[1] + in[1] * tp[0];
    sum1[0] += in[2] * tp[2] - in[3] * tp[3];
    sum1[1] += in[2] * tp[3] + in[3] * tp[2];


    in += 4;
    tp += 4;

  }


  res[0] = sum0[0] + sum1[0];
  res[1] = sum0[1] + sum1[1];



  for(i = 0; i < isodd; ++i) {


    *result += input[num_points - 1] * taps[num_points - 1];

  }

}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_SSE3

#include <pmmintrin.h>

static inline void volk_32fc_x2_dot_prod_32fc_u_sse3(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_points) {


  lv_32fc_t dotProduct;
  memset(&dotProduct, 0x0, 2*sizeof(float));

  unsigned int number = 0;
  const unsigned int halfPoints = num_points/2;

  __m128 x, y, yl, yh, z, tmp1, tmp2, dotProdVal;

  const lv_32fc_t* a = input;
  const lv_32fc_t* b = taps;

  dotProdVal = _mm_setzero_ps();

  for(;number < halfPoints; number++){

    x = _mm_loadu_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
    y = _mm_loadu_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di

    yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
    yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di

    tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr

    x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br

    tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di

    z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

    dotProdVal = _mm_add_ps(dotProdVal, z); // Add the complex multiplication results together

    a += 2;
    b += 2;
  }

  __VOLK_ATTR_ALIGNED(16) lv_32fc_t dotProductVector[2];

  _mm_storeu_ps((float*)dotProductVector,dotProdVal); // Store the results back into the dot product vector

  dotProduct += ( dotProductVector[0] + dotProductVector[1] );

  if(num_points % 1 != 0) {
    dotProduct += (*a) * (*b);
  }

  *result = dotProduct;
}

#endif /*LV_HAVE_SSE3*/

#endif /*INCLUDED_volk_32fc_x2_dot_prod_32fc_u_H*/
