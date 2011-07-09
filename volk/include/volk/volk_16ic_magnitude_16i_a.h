#ifndef INCLUDED_volk_16ic_magnitude_16i_a_H
#define INCLUDED_volk_16ic_magnitude_16i_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
/*!
  \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
  \param complexVector The vector containing the complex input values
  \param magnitudeVector The vector containing the real output values
  \param num_points The number of complex values in complexVector to be calculated and stored into cVector
*/
static inline void volk_16ic_magnitude_16i_a_sse3(int16_t* magnitudeVector, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;
   
  const int16_t* complexVectorPtr = (const int16_t*)complexVector;
  int16_t* magnitudeVectorPtr = magnitudeVector;

  __m128 vScalar = _mm_set_ps1(32768.0);
  __m128 invScalar = _mm_set_ps1(1.0/32768.0);

  __m128 cplxValue1, cplxValue2, result;

  __VOLK_ATTR_ALIGNED(16) float inputFloatBuffer[8];
  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){

    inputFloatBuffer[0] = (float)(complexVectorPtr[0]);
    inputFloatBuffer[1] = (float)(complexVectorPtr[1]);
    inputFloatBuffer[2] = (float)(complexVectorPtr[2]);
    inputFloatBuffer[3] = (float)(complexVectorPtr[3]);
      
    inputFloatBuffer[4] = (float)(complexVectorPtr[4]);
    inputFloatBuffer[5] = (float)(complexVectorPtr[5]);
    inputFloatBuffer[6] = (float)(complexVectorPtr[6]);
    inputFloatBuffer[7] = (float)(complexVectorPtr[7]);

    cplxValue1 = _mm_load_ps(&inputFloatBuffer[0]);
    cplxValue2 = _mm_load_ps(&inputFloatBuffer[4]);

    complexVectorPtr += 8;

    cplxValue1 = _mm_mul_ps(cplxValue1, invScalar);
    cplxValue2 = _mm_mul_ps(cplxValue2, invScalar);

    cplxValue1 = _mm_mul_ps(cplxValue1, cplxValue1); // Square the values
    cplxValue2 = _mm_mul_ps(cplxValue2, cplxValue2); // Square the Values

    result = _mm_hadd_ps(cplxValue1, cplxValue2); // Add the I2 and Q2 values

    result = _mm_sqrt_ps(result); // Square root the values

    result = _mm_mul_ps(result, vScalar); // Scale the results

    _mm_store_ps(outputFloatBuffer, result);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[0]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[1]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[2]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;
  magnitudeVectorPtr = &magnitudeVector[number];
  complexVectorPtr = (const int16_t*)&complexVector[number];
  for(; number < num_points; number++){
    const float val1Real = (float)(*complexVectorPtr++) / 32768.0;
    const float val1Imag = (float)(*complexVectorPtr++) / 32768.0;
    const float val1Result = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag)) * 32768.0;
    *magnitudeVectorPtr++ = (int16_t)(val1Result);
  }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
  \param complexVector The vector containing the complex input values
  \param magnitudeVector The vector containing the real output values
  \param num_points The number of complex values in complexVector to be calculated and stored into cVector
*/
static inline void volk_16ic_magnitude_16i_a_sse(int16_t* magnitudeVector, const lv_16sc_t* complexVector, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const int16_t* complexVectorPtr = (const int16_t*)complexVector;
  int16_t* magnitudeVectorPtr = magnitudeVector;

  __m128 vScalar = _mm_set_ps1(32768.0);
  __m128 invScalar = _mm_set_ps1(1.0/32768.0);

  __m128 cplxValue1, cplxValue2, iValue, qValue, result;

  __VOLK_ATTR_ALIGNED(16) float inputFloatBuffer[4];
  __VOLK_ATTR_ALIGNED(16) float outputFloatBuffer[4];

  for(;number < quarterPoints; number++){

    inputFloatBuffer[0] = (float)(complexVectorPtr[0]);
    inputFloatBuffer[1] = (float)(complexVectorPtr[1]);
    inputFloatBuffer[2] = (float)(complexVectorPtr[2]);
    inputFloatBuffer[3] = (float)(complexVectorPtr[3]);
      
    cplxValue1 = _mm_load_ps(inputFloatBuffer);
    complexVectorPtr += 4;

    inputFloatBuffer[0] = (float)(complexVectorPtr[0]);
    inputFloatBuffer[1] = (float)(complexVectorPtr[1]);
    inputFloatBuffer[2] = (float)(complexVectorPtr[2]);
    inputFloatBuffer[3] = (float)(complexVectorPtr[3]);

    cplxValue2 = _mm_load_ps(inputFloatBuffer);
    complexVectorPtr += 4;

    cplxValue1 = _mm_mul_ps(cplxValue1, invScalar);
    cplxValue2 = _mm_mul_ps(cplxValue2, invScalar);

    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    // Arrange in q1q2q3q4 format
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));

    iValue = _mm_mul_ps(iValue, iValue); // Square the I values
    qValue = _mm_mul_ps(qValue, qValue); // Square the Q Values

    result = _mm_add_ps(iValue, qValue); // Add the I2 and Q2 values

    result = _mm_sqrt_ps(result); // Square root the values

    result = _mm_mul_ps(result, vScalar); // Scale the results

    _mm_store_ps(outputFloatBuffer, result);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[0]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[1]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[2]);
    *magnitudeVectorPtr++ = (int16_t)(outputFloatBuffer[3]);
  }

  number = quarterPoints * 4;
  magnitudeVectorPtr = &magnitudeVector[number];
  complexVectorPtr = (const int16_t*)&complexVector[number];
  for(; number < num_points; number++){
    const float val1Real = (float)(*complexVectorPtr++) / 32768.0;
    const float val1Imag = (float)(*complexVectorPtr++) / 32768.0;
    const float val1Result = sqrtf((val1Real * val1Real) + (val1Imag * val1Imag)) * 32768.0;
    *magnitudeVectorPtr++ = (int16_t)(val1Result);
  }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
  \param complexVector The vector containing the complex input values
  \param magnitudeVector The vector containing the real output values
  \param num_points The number of complex values in complexVector to be calculated and stored into cVector
*/
static inline void volk_16ic_magnitude_16i_a_generic(int16_t* magnitudeVector, const lv_16sc_t* complexVector, unsigned int num_points){
  const int16_t* complexVectorPtr = (const int16_t*)complexVector;
  int16_t* magnitudeVectorPtr = magnitudeVector;
  unsigned int number = 0;
  const float scalar = 32768.0;
  for(number = 0; number < num_points; number++){
    float real = ((float)(*complexVectorPtr++)) / scalar;
    float imag = ((float)(*complexVectorPtr++)) / scalar;
    *magnitudeVectorPtr++ = (int16_t)(sqrtf((real*real) + (imag*imag)) * scalar);
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC_DISABLED
/*!
  \brief Calculates the magnitude of the complexVector and stores the results in the magnitudeVector
  \param complexVector The vector containing the complex input values
  \param magnitudeVector The vector containing the real output values
  \param num_points The number of complex values in complexVector to be calculated and stored into cVector
*/
extern void volk_16ic_magnitude_16i_a_orc_impl(int16_t* magnitudeVector, const lv_16sc_t* complexVector, float scalar, unsigned int num_points);
static inline void volk_16ic_magnitude_16i_a_orc(int16_t* magnitudeVector, const lv_16sc_t* complexVector, unsigned int num_points){
    volk_16ic_magnitude_16i_a_orc_impl(magnitudeVector, complexVector, 32768.0, num_points);
}
#endif /* LV_HAVE_ORC */


#endif /* INCLUDED_volk_16ic_magnitude_16i_a_H */
