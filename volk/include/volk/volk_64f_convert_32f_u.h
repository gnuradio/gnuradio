#ifndef INCLUDED_volk_64f_convert_32f_u_H
#define INCLUDED_volk_64f_convert_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Converts the double values into float values
    \param dVector The converted float vector values
    \param fVector The double vector values to be converted
    \param num_points The number of points in the two vectors to be converted
  */
static inline void volk_64f_convert_32f_u_sse2(float* outputVector, const double* inputVector, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const double* inputVectorPtr = (const double*)inputVector;
  float* outputVectorPtr = outputVector;
  __m128 ret, ret2;
  __m128d inputVal1, inputVal2;

  for(;number < quarterPoints; number++){
    inputVal1 = _mm_loadu_pd(inputVectorPtr); inputVectorPtr += 2;
    inputVal2 = _mm_loadu_pd(inputVectorPtr); inputVectorPtr += 2;
 
    ret = _mm_cvtpd_ps(inputVal1);
    ret2 = _mm_cvtpd_ps(inputVal2);

    ret = _mm_movelh_ps(ret, ret2);

    _mm_storeu_ps(outputVectorPtr, ret);
    outputVectorPtr += 4;
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (float)(inputVector[number]);
  }
}
#endif /* LV_HAVE_SSE2 */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Converts the double values into float values
  \param dVector The converted float vector values
  \param fVector The double vector values to be converted
  \param num_points The number of points in the two vectors to be converted
*/
static inline void volk_64f_convert_32f_u_generic(float* outputVector, const double* inputVector, unsigned int num_points){
  float* outputVectorPtr = outputVector;
  const double* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((float)(*inputVectorPtr++));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_64f_convert_32f_u_H */
