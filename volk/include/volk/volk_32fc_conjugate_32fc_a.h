#ifndef INCLUDED_volk_32fc_conjugate_32fc_a_H
#define INCLUDED_volk_32fc_conjugate_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Takes the conjugate of a complex vector.
    \param cVector The vector where the results will be stored
    \param aVector Vector to be conjugated
    \param num_points The number of complex values in aVector to be conjugated and stored into cVector
  */
static inline void volk_32fc_conjugate_32fc_a_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);

    for(;number < halfPoints; number++){
      
      x = _mm_load_ps((float*)a); // Load the complex data as ar,ai,br,bi

      x = _mm_xor_ps(x, conjugator); // conjugate register
    
      _mm_store_ps((float*)c,x); // Store the results back into the C container

      a += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = lv_conj(*a);
    }
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Takes the conjugate of a complex vector.
    \param cVector The vector where the results will be stored
    \param aVector Vector to be conjugated
    \param num_points The number of complex values in aVector to be conjugated and stored into cVector
  */
static inline void volk_32fc_conjugate_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = lv_conj(*aPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32fc_conjugate_32fc_a_H */
