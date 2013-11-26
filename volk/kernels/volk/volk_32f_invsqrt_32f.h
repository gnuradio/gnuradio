#ifndef INCLUDED_volk_32f_invsqrt_32f_a_H
#define INCLUDED_volk_32f_invsqrt_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

static inline float Q_rsqrt( float number )
{
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
 
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
        return y;
}

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Sqrts the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be invsqrted
  \param num_points The number of values in aVector and bVector to be invsqrted together and stored into cVector
*/
static inline void volk_32f_invsqrt_32f_a_sse(float* cVector, const float* aVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    float* cPtr = cVector;
    const float* aPtr = aVector;

    __m128 aVal, cVal;
    for(;number < quarterPoints; number++){

      aVal = _mm_load_ps(aPtr);

      cVal = _mm_rsqrt_ps(aVal);

      _mm_store_ps(cPtr,cVal); // Store the results back into the C container

      aPtr += 4;
      cPtr += 4;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = Q_rsqrt(*aPtr++);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Sqrts the two input vectors and store their results in the third vector
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be invsqrted
  \param num_points The number of values in aVector and bVector to be invsqrted together and stored into cVector
*/
static inline void volk_32f_invsqrt_32f_generic(float* cVector, const float* aVector, unsigned int num_points){
    float* cPtr = cVector;
    const float* aPtr = aVector;
    unsigned int number = 0;
    for(number = 0; number < num_points; number++){
        *cPtr++ = Q_rsqrt(*aPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#endif /* INCLUDED_volk_32f_invsqrt_32f_a_H */
