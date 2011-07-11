#ifndef INCLUDED_volk_32fc_s32f_x2_power_spectral_density_32f_a_H
#define INCLUDED_volk_32fc_s32f_x2_power_spectral_density_32f_a_H

#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef LV_HAVE_SSE3
#include <pmmintrin.h>

#ifdef LV_HAVE_LIB_SIMDMATH
#include <simdmath.h>
#endif /* LV_HAVE_LIB_SIMDMATH */

/*!
  \brief Calculates the log10 power value divided by the RBW for each input point
  \param logPowerOutput The 10.0 * log10((r*r + i*i)/RBW) for each data point
  \param complexFFTInput The complex data output from the FFT point
  \param normalizationFactor This value is divided against all the input values before the power is calculated
  \param rbw The resolution bandwith of the fft spectrum
  \param num_points The number of fft data points
*/
static inline void volk_32fc_s32f_x2_power_spectral_density_32f_a_sse3(float* logPowerOutput, const lv_32fc_t* complexFFTInput, const float normalizationFactor, const float rbw, unsigned int num_points){
  const float* inputPtr = (const float*)complexFFTInput;
  float* destPtr = logPowerOutput;
  uint64_t number = 0;
  const float iRBW = 1.0 / rbw;
  const float iNormalizationFactor = 1.0 / normalizationFactor;

#ifdef LV_HAVE_LIB_SIMDMATH
  __m128 magScalar = _mm_set_ps1(10.0);
  magScalar = _mm_div_ps(magScalar, logf4(magScalar));

  __m128 invRBW = _mm_set_ps1(iRBW);
  
  __m128 invNormalizationFactor = _mm_set_ps1(iNormalizationFactor);

  __m128 power;
  __m128 input1, input2;
  const uint64_t quarterPoints = num_points / 4;
  for(;number < quarterPoints; number++){
    // Load the complex values 
    input1 =_mm_load_ps(inputPtr);
    inputPtr += 4;
    input2 =_mm_load_ps(inputPtr);
    inputPtr += 4;
    
    // Apply the normalization factor
    input1 = _mm_mul_ps(input1, invNormalizationFactor);
    input2 = _mm_mul_ps(input2, invNormalizationFactor);

    // Multiply each value by itself
    // (r1*r1), (i1*i1), (r2*r2), (i2*i2)
    input1 = _mm_mul_ps(input1, input1);
    // (r3*r3), (i3*i3), (r4*r4), (i4*i4)
    input2 = _mm_mul_ps(input2, input2);
    
    // Horizontal add, to add (r*r) + (i*i) for each complex value
    // (r1*r1)+(i1*i1), (r2*r2) + (i2*i2), (r3*r3)+(i3*i3), (r4*r4)+(i4*i4)
    power = _mm_hadd_ps(input1, input2);

    // Divide by the rbw
    power = _mm_mul_ps(power, invRBW);

    // Calculate the natural log power
    power = logf4(power);
    
    // Convert to log10 and multiply by 10.0
    power = _mm_mul_ps(power, magScalar);
    
    // Store the floating point results
    _mm_store_ps(destPtr, power);
    
    destPtr += 4;
  }
  
  number = quarterPoints*4;  
#endif /* LV_HAVE_LIB_SIMDMATH */
  // Calculate the FFT for any remaining points
  for(; number < num_points; number++){
    // Calculate dBm
    // 50 ohm load assumption
    // 10 * log10 (v^2 / (2 * 50.0 * .001)) = 10 * log10( v^2 * 10)
    // 75 ohm load assumption
    // 10 * log10 (v^2 / (2 * 75.0 * .001)) = 10 * log10( v^2 * 15)
    
    const float real = *inputPtr++ * iNormalizationFactor;
    const float imag = *inputPtr++ * iNormalizationFactor;

    *destPtr = 10.0*log10f((((real * real) + (imag * imag)) + 1e-20) * iRBW);
    destPtr++;
  }
  
}
#endif /* LV_HAVE_SSE3 */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Calculates the log10 power value divided by the RBW for each input point
  \param logPowerOutput The 10.0 * log10((r*r + i*i)/RBW) for each data point
  \param complexFFTInput The complex data output from the FFT point
  \param normalizationFactor This value is divided against all the input values before the power is calculated
  \param rbw The resolution bandwith of the fft spectrum
  \param num_points The number of fft data points
*/
static inline void volk_32fc_s32f_x2_power_spectral_density_32f_a_generic(float* logPowerOutput, const lv_32fc_t* complexFFTInput, const float normalizationFactor, const float rbw, unsigned int num_points){
  // Calculate the Power of the complex point
  const float* inputPtr = (float*)complexFFTInput;
  float* realFFTDataPointsPtr = logPowerOutput;
  unsigned int point;
  const float invRBW = 1.0 / rbw;
  const float iNormalizationFactor = 1.0 / normalizationFactor;

  for(point = 0; point < num_points; point++){
    // Calculate dBm
    // 50 ohm load assumption
    // 10 * log10 (v^2 / (2 * 50.0 * .001)) = 10 * log10( v^2 * 10)
    // 75 ohm load assumption
    // 10 * log10 (v^2 / (2 * 75.0 * .001)) = 10 * log10( v^2 * 15)

    const float real = *inputPtr++ * iNormalizationFactor;
    const float imag = *inputPtr++ * iNormalizationFactor;

    *realFFTDataPointsPtr = 10.0*log10f((((real * real) + (imag * imag)) + 1e-20) * invRBW);
    
    realFFTDataPointsPtr++;
  }
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32fc_s32f_x2_power_spectral_density_32f_a_H */
