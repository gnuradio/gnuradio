#ifndef INCLUDED_volk_32f_s32f_multiply_32f_a_H
#define INCLUDED_volk_32f_s32f_multiply_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_GENERIC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
static inline void volk_32f_s32f_multiply_32f_a_generic(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
  unsigned int number = 0;
  const float* inputPtr = aVector;
  float* outputPtr = cVector;
  for(number = 0; number < num_points; number++){
    *outputPtr = (*inputPtr) * scalar;
    inputPtr++;
    outputPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */

#ifdef LV_HAVE_ORC
/*!
  \brief Scalar float multiply
  \param cVector The vector where the results will be stored
  \param aVector One of the vectors to be multiplied
  \param scalar the scalar value
  \param num_points The number of values in aVector and bVector to be multiplied together and stored into cVector
*/
extern void volk_32f_s32f_multiply_32f_a_orc_impl(float* dst, const float* src, const float scalar, unsigned int num_points);
static inline void volk_32f_s32f_multiply_32f_a_orc(float* cVector, const float* aVector, const float scalar, unsigned int num_points){
    volk_32f_s32f_multiply_32f_a_orc_impl(cVector, aVector, scalar, num_points);
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_multiply_32f_a_H */
