#ifndef INCLUDED_volk_32f_s32f_calc_spectral_noise_floor_32f_a_H
#define INCLUDED_volk_32f_s32f_calc_spectral_noise_floor_32f_a_H

#include <volk/volk_common.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief Calculates the spectral noise floor of an input power spectrum

  Calculates the spectral noise floor of an input power spectrum by determining the mean of the input power spectrum, then recalculating the mean excluding any power spectrum values that exceed the mean by the spectralExclusionValue (in dB).  Provides a rough estimation of the signal noise floor.

  \param realDataPoints The input power spectrum
  \param num_points The number of data points in the input power spectrum vector
  \param spectralExclusionValue The number of dB above the noise floor that a data point must be to be excluded from the noise floor calculation - default value is 20
  \param noiseFloorAmplitude The noise floor of the input spectrum, in dB
*/
static inline void volk_32f_s32f_calc_spectral_noise_floor_32f_a_sse(float* noiseFloorAmplitude, const float* realDataPoints, const float spectralExclusionValue, const unsigned int num_points){
  unsigned int number = 0;
  const unsigned int quarterPoints = num_points / 4;

  const float* dataPointsPtr = realDataPoints;
  __VOLK_ATTR_ALIGNED(16) float avgPointsVector[4];
    
  __m128 dataPointsVal;
  __m128 avgPointsVal = _mm_setzero_ps();
  // Calculate the sum (for mean) for all points
  for(; number < quarterPoints; number++){

    dataPointsVal = _mm_load_ps(dataPointsPtr);

    dataPointsPtr += 4;

    avgPointsVal = _mm_add_ps(avgPointsVal, dataPointsVal);
  }

  _mm_store_ps(avgPointsVector, avgPointsVal);

  float sumMean = 0.0;
  sumMean += avgPointsVector[0];
  sumMean += avgPointsVector[1];
  sumMean += avgPointsVector[2];
  sumMean += avgPointsVector[3];

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    sumMean += realDataPoints[number];
  }

  // calculate the spectral mean
  // +20 because for the comparison below we only want to throw out bins
  // that are significantly higher (and would, thus, affect the mean more
  const float meanAmplitude = (sumMean / ((float)num_points)) + spectralExclusionValue;

  dataPointsPtr = realDataPoints; // Reset the dataPointsPtr
  __m128 vMeanAmplitudeVector = _mm_set_ps1(meanAmplitude);
  __m128 vOnesVector = _mm_set_ps1(1.0);
  __m128 vValidBinCount = _mm_setzero_ps();
  avgPointsVal = _mm_setzero_ps();
  __m128 compareMask;
  number = 0;
  // Calculate the sum (for mean) for any points which do NOT exceed the mean amplitude
  for(; number < quarterPoints; number++){

    dataPointsVal = _mm_load_ps(dataPointsPtr);

    dataPointsPtr += 4;

    // Identify which items do not exceed the mean amplitude
    compareMask = _mm_cmple_ps(dataPointsVal, vMeanAmplitudeVector);

    // Mask off the items that exceed the mean amplitude and add the avg Points that do not exceed the mean amplitude
    avgPointsVal = _mm_add_ps(avgPointsVal, _mm_and_ps(compareMask, dataPointsVal));
      
    // Count the number of bins which do not exceed the mean amplitude
    vValidBinCount = _mm_add_ps(vValidBinCount, _mm_and_ps(compareMask, vOnesVector));
  }
    
  // Calculate the mean from the remaining data points
  _mm_store_ps(avgPointsVector, avgPointsVal);

  sumMean = 0.0;
  sumMean += avgPointsVector[0];
  sumMean += avgPointsVector[1];
  sumMean += avgPointsVector[2];
  sumMean += avgPointsVector[3];

  // Calculate the number of valid bins from the remaning count
  __VOLK_ATTR_ALIGNED(16) float validBinCountVector[4];
  _mm_store_ps(validBinCountVector, vValidBinCount);

  float validBinCount = 0;
  validBinCount += validBinCountVector[0];
  validBinCount += validBinCountVector[1];
  validBinCount += validBinCountVector[2];
  validBinCount += validBinCountVector[3];

  number = quarterPoints * 4;
  for(;number < num_points; number++){
    if(realDataPoints[number] <= meanAmplitude){
      sumMean += realDataPoints[number];
      validBinCount += 1.0;
    }
  }
    
  float localNoiseFloorAmplitude = 0;
  if(validBinCount > 0.0){
    localNoiseFloorAmplitude = sumMean / validBinCount;
  }
  else{
    localNoiseFloorAmplitude = meanAmplitude; // For the odd case that all the amplitudes are equal...
  }

  *noiseFloorAmplitude = localNoiseFloorAmplitude;
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief Calculates the spectral noise floor of an input power spectrum

  Calculates the spectral noise floor of an input power spectrum by determining the mean of the input power spectrum, then recalculating the mean excluding any power spectrum values that exceed the mean by the spectralExclusionValue (in dB).  Provides a rough estimation of the signal noise floor.

  \param realDataPoints The input power spectrum
  \param num_points The number of data points in the input power spectrum vector
  \param spectralExclusionValue The number of dB above the noise floor that a data point must be to be excluded from the noise floor calculation - default value is 20
  \param noiseFloorAmplitude The noise floor of the input spectrum, in dB
*/
static inline void volk_32f_s32f_calc_spectral_noise_floor_32f_a_generic(float* noiseFloorAmplitude, const float* realDataPoints, const float spectralExclusionValue, const unsigned int num_points){
  float sumMean = 0.0;
  unsigned int number;
  // find the sum (for mean), etc
  for(number = 0; number < num_points; number++){
    // sum (for mean)
    sumMean += realDataPoints[number];
  }

  // calculate the spectral mean
  // +20 because for the comparison below we only want to throw out bins
  // that are significantly higher (and would, thus, affect the mean more)
  const float meanAmplitude = (sumMean / num_points) + spectralExclusionValue;

  // now throw out any bins higher than the mean
  sumMean = 0.0;
  unsigned int newNumDataPoints = num_points;
  for(number = 0; number < num_points; number++){
    if (realDataPoints[number] <= meanAmplitude)
      sumMean += realDataPoints[number];
    else
      newNumDataPoints--;
  }

  float localNoiseFloorAmplitude = 0.0;
  if (newNumDataPoints == 0)             // in the odd case that all
    localNoiseFloorAmplitude = meanAmplitude; // amplitudes are equal!
  else
    localNoiseFloorAmplitude = sumMean / ((float)newNumDataPoints);

  *noiseFloorAmplitude = localNoiseFloorAmplitude;
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_calc_spectral_noise_floor_32f_a_H */
