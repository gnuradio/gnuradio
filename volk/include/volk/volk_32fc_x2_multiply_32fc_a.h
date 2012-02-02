#ifndef INCLUDED_volk_32fc_x2_multiply_32fc_a_H
#define INCLUDED_volk_32fc_x2_multiply_32fc_a_H

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
static inline void volk_32fc_x2_multiply_32fc_a_sse3(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
  unsigned int number = 0;
    const unsigned int halfPoints = num_points / 2;

    __m128 x, y, yl, yh, z, tmp1, tmp2;
    lv_32fc_t* c = cVector;
    const lv_32fc_t* a = aVector;
    const lv_32fc_t* b = bVector;
    for(;number < halfPoints; number++){
      
      x = _mm_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
      y = _mm_load_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di
      
      yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
      yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di
      
      tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr
      
      x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br
      
      tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di
      
      z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di
    
      _mm_store_ps((float*)c,z); // Store the results back into the C container

      a += 2;
      b += 2;
      c += 2;
    }

    if((num_points % 2) != 0) {
      *c = (*a) * (*b);
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
static inline void volk_32fc_x2_multiply_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    const lv_32fc_t* bPtr=  bVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
extern void volk_32fc_x2_multiply_32fc_a_orc_impl(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points);
static inline void volk_32fc_x2_multiply_32fc_a_orc(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t* bVector, unsigned int num_points){
    volk_32fc_x2_multiply_32fc_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_ORC */





#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_a_H */
