#ifndef INCLUDED_volk_32fc_s32f_power_32fc_a16_H
#define INCLUDED_volk_32fc_s32f_power_32fc_a16_H

#include <inttypes.h>
#include <stdio.h>

#if LV_HAVE_SSE
#include <xmmintrin.h>

#if LV_HAVE_LIB_SIMDMATH
#include <simdmath.h>
#endif /* LV_HAVE_LIB_SIMDMATH */

/*!
  \brief Takes each the input complex vector value to the specified power and stores the results in the return vector
  \param cVector The vector where the results will be stored
  \param aVector The complex vector of values to be taken to a power
  \param power The power value to be applied to each data point
  \param num_points The number of values in aVector to be taken to the specified power level and stored into cVector
*/
static inline void volk_32fc_s32f_power_32fc_a16_sse(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float power, unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;
  
  lv_32fc_t* cPtr = cVector;
  const lv_32fc_t* aPtr = aVector;

#if LV_HAVE_LIB_SIMDMATH
  __m128 vPower = _mm_set_ps1(power);
  
  __m128 cplxValue1, cplxValue2, magnitude, phase, iValue, qValue;
  for(;number < quarterPoints; number++){
    
    cplxValue1 = _mm_load_ps((float*)aPtr); 
    aPtr += 2;
    
    cplxValue2 = _mm_load_ps((float*)aPtr); 
    aPtr += 2;
    
    // Convert to polar coordinates
    
    // Arrange in i1i2i3i4 format
    iValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(2,0,2,0));
    // Arrange in q1q2q3q4 format
    qValue = _mm_shuffle_ps(cplxValue1, cplxValue2, _MM_SHUFFLE(3,1,3,1));
    
    phase = atan2f4(qValue, iValue); // Calculate the Phase
    
    magnitude = _mm_sqrt_ps(_mm_add_ps(_mm_mul_ps(iValue, iValue), _mm_mul_ps(qValue, qValue))); // Calculate the magnitude by square rooting the added I2 and Q2 values
    
    // Now calculate the power of the polar coordinate data
    magnitude = powf4(magnitude, vPower); // Take the magnitude to the specified power
    
    phase = _mm_mul_ps(phase, vPower); // Multiply the phase by the specified power
    
    // Convert back to cartesian coordinates
    iValue = _mm_mul_ps( cosf4(phase), magnitude); // Multiply the cos of the phase by the magnitude
    qValue = _mm_mul_ps( sinf4(phase), magnitude); // Multiply the sin of the phase by the magnitude
    
    cplxValue1 = _mm_unpacklo_ps(iValue, qValue); // Interleave the lower two i & q values
    cplxValue2 = _mm_unpackhi_ps(iValue, qValue); // Interleave the upper two i & q values
    
    _mm_store_ps((float*)cPtr,cplxValue1); // Store the results back into the C container
    
    cPtr += 2;
    
    _mm_store_ps((float*)cPtr,cplxValue2); // Store the results back into the C container
    
    cPtr += 2;
  }
  
  number = quarterPoints * 4;
#endif /* LV_HAVE_LIB_SIMDMATH */

  lv_32fc_t complexPower;
  ((float*)&complexPower)[0] = power;
  ((float*)&complexPower)[1] = 0;
  for(;number < num_points; number++){
    *cPtr++ = lv_cpow((*aPtr++), complexPower);
  }
}
#endif /* LV_HAVE_SSE */

#if LV_HAVE_GENERIC
  /*!
    \brief Takes each the input complex vector value to the specified power and stores the results in the return vector
    \param cVector The vector where the results will be stored
    \param aVector The complex vector of values to be taken to a power
    \param power The power value to be applied to each data point
    \param num_points The number of values in aVector to be taken to the specified power level and stored into cVector
  */
static inline void volk_32fc_s32f_power_32fc_a16_generic(lv_32fc_t* cVector, const lv_32fc_t* aVector, const float power, unsigned int num_points){
  lv_32fc_t* cPtr = cVector;
  const lv_32fc_t* aPtr = aVector;
  unsigned int number = 0;
  lv_32fc_t complexPower;
  ((float*)&complexPower)[0] = power;
  ((float*)&complexPower)[1] = 0.0;

  for(number = 0; number < num_points; number++){
    *cPtr++ = lv_cpow((*aPtr++), complexPower);
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_s32f_power_32fc_a16_H */
