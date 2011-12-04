#ifndef INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H
#define INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Multiplys the one complex vector with the complex conjugate of the second complex vector and stores their results in the third vector
  \param cVector The complex vector where the results will be stored
  \param aVector One of the complex vectors to be multiplied
  \param bVector The complex vector which will be converted to complex conjugate and multiplied
  \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_8ic_x2_s32f_multiply_conjugate_32fc_a_sse4_1(lv_32fc_t* cVector, const lv_8sc_t* aVector, const lv_8sc_t* bVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  __m128i x, y, realz, imagz;
  __m128 ret;
  lv_32fc_t* c = cVector;
  const lv_8sc_t* a = aVector;
  const lv_8sc_t* b = bVector;
  __m128i conjugateSign = _mm_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1);

  __m128 invScalar = _mm_set_ps1(1.0/scalar);

  for(;number < quarterPoints; number++){
    // Convert into 8 bit values into 16 bit values
    x = _mm_cvtepi8_epi16(_mm_loadl_epi64((__m128i*)a));
    y = _mm_cvtepi8_epi16(_mm_loadl_epi64((__m128i*)b));

    // Calculate the ar*cr - ai*(-ci) portions
    realz = _mm_madd_epi16(x,y);

    // Calculate the complex conjugate of the cr + ci j values
    y = _mm_sign_epi16(y, conjugateSign);

    // Shift the order of the cr and ci values
    y = _mm_shufflehi_epi16(_mm_shufflelo_epi16(y, _MM_SHUFFLE(2,3,0,1) ), _MM_SHUFFLE(2,3,0,1));

    // Calculate the ar*(-ci) + cr*(ai)
    imagz = _mm_madd_epi16(x,y);

    // Interleave real and imaginary and then convert to float values
    ret = _mm_cvtepi32_ps(_mm_unpacklo_epi32(realz, imagz));

    // Normalize the floating point values
    ret = _mm_mul_ps(ret, invScalar);

    // Store the floating point values
    _mm_store_ps((float*)c, ret);
    c += 2;

    // Interleave real and imaginary and then convert to float values
    ret = _mm_cvtepi32_ps(_mm_unpackhi_epi32(realz, imagz));

    // Normalize the floating point values
    ret = _mm_mul_ps(ret, invScalar);

    // Store the floating point values
    _mm_store_ps((float*)c, ret);
    c += 2;

    a += 4;
    b += 4;
  }

  number = quarterPoints * 4;
  float* cFloatPtr = (float*)&cVector[number];
  int8_t* a8Ptr = (int8_t*)&aVector[number];
  int8_t* b8Ptr = (int8_t*)&bVector[number];
  for(; number < num_points; number++){
    float aReal =  (float)*a8Ptr++;
    float aImag =  (float)*a8Ptr++;
    lv_32fc_t aVal = lv_cmake(aReal, aImag );
    float bReal = (float)*b8Ptr++;
    float bImag = (float)*b8Ptr++;
    lv_32fc_t bVal = lv_cmake( bReal, -bImag );
    lv_32fc_t temp = aVal * bVal;
    
    *cFloatPtr++ = lv_creal(temp) / scalar;
    *cFloatPtr++ = lv_cimag(temp) / scalar;
  }
}
#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Multiplys the one complex vector with the complex conjugate of the second complex vector and stores their results in the third vector
  \param cVector The complex vector where the results will be stored
  \param aVector One of the complex vectors to be multiplied
  \param bVector The complex vector which will be converted to complex conjugate and multiplied
  \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_8ic_x2_s32f_multiply_conjugate_32fc_a_generic(lv_32fc_t* cVector, const lv_8sc_t* aVector, const lv_8sc_t* bVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  float* cPtr = (float*)cVector;
  const float invScalar = 1.0 / scalar;
  int8_t* a8Ptr = (int8_t*)aVector;
  int8_t* b8Ptr = (int8_t*)bVector;
  for(number = 0; number < num_points; number++){
    float aReal =  (float)*a8Ptr++;
    float aImag =  (float)*a8Ptr++;
    lv_32fc_t aVal = lv_cmake(aReal, aImag );
    float bReal = (float)*b8Ptr++;
    float bImag = (float)*b8Ptr++;
    lv_32fc_t bVal = lv_cmake( bReal, -bImag );
    lv_32fc_t temp = aVal * bVal;
    
    *cPtr++ = (lv_creal(temp) * invScalar);
    *cPtr++ = (lv_cimag(temp) * invScalar);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_x2_s32f_multiply_conjugate_32fc_a_H */
