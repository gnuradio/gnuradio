#ifndef INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_multiply_32fc_a_H

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
static inline void volk_32fc_s32fc_multiply_32fc_a_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    lv_32fc_t* cPtr = cVector;
    const lv_32fc_t* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *cPtr++ = (*aPtr++) * scalar;
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
extern void volk_32fc_s32fc_multiply_32fc_a_orc_impl(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points);
static inline void volk_32fc_s32fc_multiply_32fc_a_orc(lv_32fc_t* cVector, const lv_32fc_t* aVector, const lv_32fc_t scalar, unsigned int num_points){
    volk_32fc_s32fc_multiply_32fc_a_orc_impl(cVector, aVector, scalar, num_points);
}
#endif /* LV_HAVE_ORC */





#endif /* INCLUDED_volk_32fc_x2_multiply_32fc_a_H */
