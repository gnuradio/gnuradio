#ifndef INCLUDED_volk_32f_x2_divide_32f_a_H
#define INCLUDED_volk_32f_x2_divide_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Divides the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be divideed
  \param bVector The divisor vector
  \param num_points The number of values in aVector and bVector to be divideed together and stored into cVector
*/
static inline void volk_32f_x2_divide_32f_a_sse(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal, bVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);
      bVal = _mm_load_ps(bPtr);

      cVal = _mm_div_ps(aVal, bVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      bPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) / (*bPtr++);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Divides the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be divideed
  \param bVector The divisor vector
  \param num_points The number of values in aVector and bVector to be divideed together and stored into cVector
*/
static inline void volk_32f_x2_divide_32f_generic(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    const float* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) / (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Divides the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector The vector to be divideed
  \param bVector The divisor vector
  \param num_points The number of values in aVector and bVector to be divideed together and stored into cVector
*/
extern void volk_32f_x2_divide_32f_a_orc_impl(float* cVector, const float* aVector, const float* bVector, unsigned int num_points);
static inline void volk_32f_x2_divide_32f_u_orc(float* cVector, const float* aVector, const float* bVector, unsigned int num_points){
    volk_32f_x2_divide_32f_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_ORC */



#endif /* INCLUDED_volk_32f_x2_divide_32f_a_H */
