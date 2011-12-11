#ifndef INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H
#define INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H


#include<volk/volk_complex.h>


#ifdef LV_HAVE_GENERIC


static inline void volk_32fc_x2_conjugate_dot_prod_32fc_u_generic(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {
  
  float * res = (float*) result;
  float * in = (float*) input;
  float * tp = (float*) taps;
  unsigned int n_2_ccomplex_blocks = num_bytes >> 4;
  unsigned int isodd = (num_bytes >> 3) &1;
  
  
  
  float sum0[2] = {0,0};
  float sum1[2] = {0,0};
  unsigned int i = 0;

  
  for(i = 0; i < n_2_ccomplex_blocks; ++i) {
    
    sum0[0] += in[0] * tp[0] + in[1] * tp[1];
    sum0[1] += (-in[0] * tp[1]) + in[1] * tp[0];
    sum1[0] += in[2] * tp[2] + in[3] * tp[3];
    sum1[1] += (-in[2] * tp[3]) + in[3] * tp[2];
    
    
    in += 4;
    tp += 4;

  }
 
  
  res[0] = sum0[0] + sum1[0];
  res[1] = sum0[1] + sum1[1];
  
  
  
  for(i = 0; i < isodd; ++i) {


    *result += input[(num_bytes >> 3) - 1] * lv_conj(taps[(num_bytes >> 3) - 1]);

  }
  /*
  for(i = 0; i < num_bytes >> 3; ++i) {
    *result += input[i] * conjf(taps[i]);
  }
  */
}

#endif /*LV_HAVE_GENERIC*/

#ifdef LV_HAVE_SSE3

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <mmintrin.h>


static inline void volk_32fc_x2_conjugate_dot_prod_32fc_u_sse3(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {

  // Variable never used?
  //__VOLK_ATTR_ALIGNED(16) static const uint32_t conjugator[4]= {0x00000000, 0x80000000, 0x00000000, 0x80000000};

  union HalfMask {
    uint32_t intRep[4];
    __m128 vec;
    } halfMask;
 
  union NegMask {
    int intRep[4];
    __m128 vec;
  } negMask;

  unsigned int offset = 0;
  float Rsum=0, Isum=0;
  float Im,Re;

  __m128 in1, in2, Rv, fehg, Iv, Rs, Ivm, Is;
  __m128 zv = {0,0,0,0};
  
  halfMask.intRep[0] = halfMask.intRep[1] = 0xFFFFFFFF;
  halfMask.intRep[2] = halfMask.intRep[3] = 0x00000000;

  negMask.intRep[0] = negMask.intRep[2] = 0x80000000;
  negMask.intRep[1] = negMask.intRep[3] = 0;
  
  // main loop
  while(num_bytes >= 4*sizeof(float)){

    in1 = _mm_loadu_ps( (float*) (input+offset) );
    in2 = _mm_loadu_ps( (float*) (taps+offset) );
    Rv = _mm_mul_ps(in1, in2);
    fehg = _mm_shuffle_ps(in2, in2, _MM_SHUFFLE(2,3,0,1));
    Iv = _mm_mul_ps(in1, fehg);
    Rs = _mm_hadd_ps( _mm_hadd_ps(Rv, zv) ,zv);
    Ivm = _mm_xor_ps( negMask.vec, Iv );
    Is = _mm_hadd_ps( _mm_hadd_ps(Ivm, zv) ,zv);
    _mm_store_ss( &Im, Is );
    _mm_store_ss( &Re, Rs );
    num_bytes -= 4*sizeof(float);
    offset += 2;
    Rsum += Re;
    Isum += Im;
  }

  // handle the last complex case ...
  if(num_bytes > 0){

    if(num_bytes != 4){
      // bad things are happening
    }

    in1 = _mm_loadu_ps( (float*) (input+offset) );
    in2 = _mm_loadu_ps( (float*) (taps+offset) );
    Rv = _mm_and_ps(_mm_mul_ps(in1, in2), halfMask.vec);
    fehg = _mm_shuffle_ps(in2, in2, _MM_SHUFFLE(2,3,0,1));
    Iv = _mm_and_ps(_mm_mul_ps(in1, fehg), halfMask.vec);
    Rs = _mm_hadd_ps(_mm_hadd_ps(Rv, zv),zv);
    Ivm = _mm_xor_ps( negMask.vec, Iv );
    Is = _mm_hadd_ps(_mm_hadd_ps(Ivm, zv),zv);
    _mm_store_ss( &Im, Is );
    _mm_store_ss( &Re, Rs );
    Rsum += Re;
    Isum += Im;
  }

  result[0] = lv_cmake(Rsum,Isum);
  return;
}

#endif /*LV_HAVE_SSE3*/


#endif /*INCLUDED_volk_32fc_x2_conjugate_dot_prod_32fc_u_H*/



