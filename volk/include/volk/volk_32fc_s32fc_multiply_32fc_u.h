#ifndef INCLUDED_volk_32fc_s32fc_multiply_32fc_u_H
#define INCLUDED_volk_32fc_s32fc_multiply_32fc_u_H

#include <inttypes.h>
#include <stdio.h>
#include <volk/volk_complex.h>
#include <float.h>

#ifdef LV_HAVE_GENERIC
  /*!
    \brief Multiplies the two input complex vectors and stores their results in the third vector
    \param cVector The vector where the results will be stored
    \param aVector One of the vectors to be multiplied
    \param bVector One of the vectors to be multiplied
    \param num_points The number of complex values in aVector and bVector to be multiplied together and stored into cVector
  */
static inline void volk_32fc_s32fc_multiply_32fc_u_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
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


#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_u_H */
