#ifndef INCLUDED_volk_8ic_x2_multiply_conjugate_16ic_a_H
#define INCLUDED_volk_8ic_x2_multiply_conjugate_16ic_a_H

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
static inline void volk_8ic_x2_multiply_conjugate_16ic_a_sse4_1(lv_16sc_t* cVector, const lv_8sc_t* aVector, const lv_8sc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  __m128i x, y, realz, imagz;
  lv_16sc_t* c = cVector;
  const lv_8sc_t* a = aVector;
  const lv_8sc_t* b = bVector;
  __m128i conjugateSign = _mm_set_epi16(-1, 1, -1, 1, -1, 1, -1, 1);
    
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

    _mm_store_si128((__m128i*)c, _mm_packs_epi32(_mm_unpacklo_epi32(realz, imagz), _mm_unpackhi_epi32(realz, imagz)));

    a += 4;
    b += 4;
    c += 4;
  }
    
  number = quarterPoints * 4;
  int16_t* c16Ptr = (int16_t*)&cVector[number];
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

    *c16Ptr++ = (int16_t)lv_creal(temp);
    *c16Ptr++ = (int16_t)lv_cimag(temp);
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
static inline void volk_8ic_x2_multiply_conjugate_16ic_a_generic(lv_16sc_t* cVector, const lv_8sc_t* aVector, const lv_8sc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
  int16_t* c16Ptr = (int16_t*)cVector;
  int8_t* a8Ptr = (int8_t*)aVector;
  int8_t* b8Ptr = (int8_t*)bVector;
  for(number =0; number < num_points; number++){
    float aReal =  (float)*a8Ptr++;
    float aImag =  (float)*a8Ptr++;
    lv_32fc_t aVal = lv_cmake(aReal, aImag );
    float bReal = (float)*b8Ptr++;
    float bImag = (float)*b8Ptr++;
    lv_32fc_t bVal = lv_cmake( bReal, -bImag );
    lv_32fc_t temp = aVal * bVal;

    *c16Ptr++ = (int16_t)lv_creal(temp);
    *c16Ptr++ = (int16_t)lv_cimag(temp);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_8ic_x2_multiply_conjugate_16ic_a_H */
