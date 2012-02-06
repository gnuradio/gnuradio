#ifndef INCLUDED_volk_32f_x2_add_32f_u_H
#define INCLUDED_volk_32f_x2_add_32f_u_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Adds the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be added
  \param bVector One of the vectors to be added
  \param num_points The number of values in aVector and bVector to be added together and stored into cVector
*/
static inline void volk_32f_x2_add_32f_u_sse(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){
      
      aVal = _mm_loadu_ps(aPtr); 
      bVal = _mm_loadu_ps(bPtr);
      
      cVal = _mm_add_ps(aVal, bVal); 
      
      _mm_storeu_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) + (*bPtr++);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Adds the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be added
  \param bVector One of the vectors to be added
  \param num_points The number of values in aVector and bVector to be added together and stored into cVector
*/
static inline void volk_32f_x2_add_32f_u_generic(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) + (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_x2_add_32f_u_H */
