#ifndef INCLUDED_volk_32f_convert_64f_u_H
#define INCLUDED_volk_32f_convert_64f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
  /*!
    \brief Converts the float values into double values
    \param dVector The converted double vector values
    \param fVector The float vector values to be converted
    \param num_points The number of points in the two vectors to be converted
  */
static inline void volk_32f_convert_64f_u_sse2(double* outputVector, const float* inputVector, unsigned int num_points){
  unsigned int number = 0;

  const unsigned int quarterPoints = num_points / 4;
    
  const float* inputVectorPtr = (const float*)inputVector;
  double* outputVectorPtr = outputVector;
  __m128d ret;
  __m128 inputVal;

  for(;number < quarterPoints; number++){
    inputVal = _mm_loadu_ps(inputVectorPtr); inputVectorPtr += 4;
 
    ret = _mm_cvtps_pd(inputVal);

    _mm_storeu_pd(outputVectorPtr, ret);
    outputVectorPtr += 2;

    inputVal = _mm_movehl_ps(inputVal, inputVal);

    ret = _mm_cvtps_pd(inputVal);

    _mm_storeu_pd(outputVectorPtr, ret);
    outputVectorPtr += 2;
  }

  number = quarterPoints * 4;    
  for(; number < num_points; number++){
    outputVector[number] = (double)(inputVector[number]);
  }
}
#endif /* LV_HAVE_SSE2 */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Converts the float values into double values
  \param dVector The converted double vector values
  \param fVector The float vector values to be converted
  \param num_points The number of points in the two vectors to be converted
*/
static inline void volk_32f_convert_64f_u_generic(double* outputVector, const float* inputVector, unsigned int num_points){
  double* outputVectorPtr = outputVector;
  const float* inputVectorPtr = inputVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *outputVectorPtr++ = ((double)(*inputVectorPtr++));
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_convert_64f_u_H */
