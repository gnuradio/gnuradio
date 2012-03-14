#ifndef INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_a_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;

    // Set up constant scalar vector
    yl = _mm_set_ps1(lv_creal(scalar));
    yh = _mm_set_ps1(lv_cimag(scalar));

    for(;number < halfPoints; number++){
      
      x = _mm_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
      
      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr
      
      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br
      
      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di
      
      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di
    
      _mm_store_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * scalar;
    }
}
#endif /* LV_HAVE_SSE */


#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = num_points;

    // unwrap loop
    while (number >= 8){
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      *cPtr++ = (*aPtr++) * scalar;
      number -= 8;
    }

    // clean up any remaining
    while (number-- > 0)
      *cPtr++ = *aPtr++ * scalar;
}
#endif /* LV_HAVE_GENERIC */





#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_a_H */
