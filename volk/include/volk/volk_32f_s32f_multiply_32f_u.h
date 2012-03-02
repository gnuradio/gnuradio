#ifndef INCLUDED_volk_32f_s32f_multiply_32f_u_H
#define INCLUDED_volk_32f_s32f_multiply_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_sse(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m128 aVal, bVal, cVal;
    bVal = _mm_set_ps1(scalar);
    for(;number < quarterPoints; number++){
      
      aVal = _mm_loadu_ps(aPtr); 
      
      cVal = _mm_mul_ps(aVal, bVal); 
      
      _mm_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_AVX
#include <immintrin.h>
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_avx(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int eighthPoints = num_points / 8;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m256 aVal, bVal, cVal;
    bVal = _mm256_set1_ps(scalar);
    for(;number < eighthPoints; number++){
      
      aVal = _mm256_loadu_ps(aPtr); 
      
      cVal = _mm256_mul_ps(aVal, bVal); 
      
      _mm256_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 8;
      cPtr += 8;
    }

    number = eighthPoints * 8;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
    }
}
#endif /* LV_HAVE_AVX */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_u_generic(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const float* inputPtr = aVector;
  float* outputPtr = cVector;
  for(number = 0; number < num_points; number++){
    *outputPtr = (*inputPtr) * scalar;
    inputPtr++;
    outputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32f_s32f_multiply_32f_u_H */
