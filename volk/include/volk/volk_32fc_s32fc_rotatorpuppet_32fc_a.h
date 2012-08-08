#ifndef INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H


#include <volk/volk_complex.h>
#include <stdio.h>
#include <volk/volk_32fc_s32fc_x2_rotator_32fc_a.h>


#ifdef LV_HAVE_GENERIC

/*!
  \brief rotate input vector at fixed rate per sample from initial phase offset
  \param outVector The vector where the results will be stored
  \param inVector Vector to be rotated
  \param phase_inc rotational velocity
  \param phase initial phase offset
  \param num_points The number of values in inVector to be rotated and stored into cVector
*/


static inline void volk_32fc_s32fc_rotatorpuppet_32fc_a_generic(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){    
    lv_32fc_t phase[1] = {lv_cmake(.3, 0.95393)};
    volk_32fc_s32fc_x2_rotator_32fc_a_generic(outVector, inVector, phase_inc, phase, num_points);
    
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void volk_32fc_s32fc_rotatorpuppet_32fc_a_sse4_1(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){    
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_a_sse4_1(outVector, inVector, phase_inc, phase, num_points);
    
}




#endif /* LV_HAVE_SSE4_1 */

#ifdef LV_HAVE_AVX
#include <immintrin.h>

/*!
  \brief rotate input vector at fixed rate per sample from initial phase offset
  \param outVector The vector where the results will be stored
  \param inVector Vector to be rotated
  \param phase_inc rotational velocity
  \param phase initial phase offset
  \param num_points The number of values in inVector to be rotated and stored into cVector
*/




static inline void volk_32fc_s32fc_rotatorpuppet_32fc_a_avx(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, unsigned int num_points){    
    lv_32fc_t phase[1] = {lv_cmake(.3, .95393)};
    volk_32fc_s32fc_x2_rotator_32fc_a_avx(outVector, inVector, phase_inc, phase, num_points);
    
}
    
#endif /* LV_HAVE_AVX */








#endif /* INCLUDED_volk_32fc_s32fc_rotatorpuppet_32fc_a_H */
