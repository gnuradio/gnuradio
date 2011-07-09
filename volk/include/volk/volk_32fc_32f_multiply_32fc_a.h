#ifndef INCLUDED_volk_32fc_32f_multiply_32fc_a_H
#define INCLUDED_volk_32fc_32f_multiply_32fc_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
  /*!
    \brief Multiplies the input complex vector with the input float vector and store their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector The complex vector to be multiplied
    \param bVector The vectors containing the float values to be multiplied against each complex value in aVector
    \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_32f_multiply_32fc_a_sse(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float* bVector, unsigned int num_points){
    unsigned int number = 0;
    const unsigned int quarterPoints = num_points / 4;

    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    const float* bPtr=  bVector;

    __m128 aVal1, aVal2, bVal, bVal1, bVal2, cVal;
    for(;number < quarterPoints; number++){
      
      aVal1 = _mm_load_ps((const float*)aPtr);
      aPtr += 2;
 
      aVal2 = _mm_load_ps((const float*)aPtr); 
      aPtr += 2;

      bVal = _mm_load_ps(bPtr);
      bPtr += 4;

      bVal1 = _mm_shuffle_ps(bVal, bVal, _MM_SHUFFLE(1,1,0,0));
      bVal2 = _mm_shuffle_ps(bVal, bVal, _MM_SHUFFLE(3,3,2,2));

      cVal = _mm_mul_ps(aVal1, bVal1); 
      
      _mm_store_ps((float*)cPtr,cVal); // Store the results back into the C container
      cPtr += 2;

      cVal = _mm_mul_ps(aVal2, bVal2); 
      
      _mm_store_ps((float*)cPtr,cVal); // Store the results back into the C container

      cPtr += 2;
    }

    number = quarterPoints * 4;
    for(;number < num_points; number++){
      *cPtr++ = (*aPtr++) * (*bPtr);
      bPtr++;
    }
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies the input complex vector with the input lv_32fc_t vector and store their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector The complex vector to be multiplied
    \param bVector The vectors containing the lv_32fc_t values to be multiplied against each complex value in aVector
    \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_32f_multiply_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float* bVector, unsigned int num_points){
  lv_32fc_t* cPtr = cVector;
  const lv_32fc_t* aPtr = aVector;
  const float* bPtr=  bVector;
  unsigned int number = 0;
  
  for(number = 0; number < num_points; number++){
    *cPtr++ = (*aPtr++) * (*bPtr++);
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
  /*!
    \brief Multiplies the input complex vector with the input lv_32fc_t vector and store their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector The complex vector to be multiplied
    \param bVector The vectors containing the lv_32fc_t values to be multiplied against each complex value in aVector
    \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
  */
extern void volk_32fc_32f_multiply_32fc_a_orc_impl(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float* bVector, unsigned int num_points);
static inline void volk_32fc_32f_multiply_32fc_a_orc(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float* bVector, unsigned int num_points){
    volk_32fc_32f_multiply_32fc_a_orc_impl(cVector, aVector, bVector, num_points);
}
#endif /* LV_HAVE_GENERIC */



#endif /* INCLUDED_volk_32fc_32f_multiply_32fc_a_H */
