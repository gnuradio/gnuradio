#ifndef INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H
#define INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_u_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;

    __m128 conjugator = _mm_setr_ps(0, -0.f, 0, -0.f);

    for(;number < halfPoints; number++){
      
      x = _mm_loadu_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
      y = _mm_loadu_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di

      y = _mm_xor_ps(y, conjugator); // conjugate y
      
      yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
      yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di
      
      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr
      
      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br
      
      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di
      
      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di
    
      _mm_storeu_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      b += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * lv_conj(*b);
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies vector a by the conjugate of vector b and stores the results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector First vector to be multiplied
    \param bVector Second vector that is conjugated before being multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_x2_multiply_conjugate_32fc_u_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    const lv_32fc_t* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * lv_conj(*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#endif /* INCLUDED_volk_32fc_x2_multiply_conjugate_32fc_u_H */
