#ifndef INCLUDED_volk_32f_s32f_32f_fm_detect_32f_a_H
#define INCLUDED_volk_32f_s32f_32f_fm_detect_32f_a_H

#include <inttypes.h>
#include <stdio.h>

#ifdef LV_HAVE_SSE
#include <xmmintrin.h>
/*!
  \brief performs the FM-detect differentiation on the input vector and stores the results in the output vector.
  \param outputVector The byte-aligned vector where the results will be stored.
  \param inputVector The byte-aligned input vector containing phase data (must be on the interval (-bound,bound] )
  \param bound The interval that the input phase data is in, which is used to modulo the differentiation
  \param saveValue A pointer to a float which contains the phase value of the sample before the first input sample.
  \param num_noints The number of real values in the input vector.
*/
static inline void volk_32f_s32f_32f_fm_detect_32f_a_sse(float* outputVector, const float* inputVector, const float bound, float* saveValue, unsigned int num_points){
  if (num_points < 1) {
    return;
  }
  unsigned int number = 1;
  unsigned int j = 0;
  // num_points-1 keeps Fedora 7's gcc from crashing...
  // num_points won't work.  :(
  const unsigned int quarterPoints = (num_points-1) / 4;

  float* outPtr = outputVector;
  const float* inPtr = inputVector;
  __m128 upperBound = _mm_set_ps1(bound);
  __m128 lowerBound = _mm_set_ps1(-bound);
  __m128 next3old1;
  __m128 next4;
  __m128 boundAdjust;
  __m128 posBoundAdjust = _mm_set_ps1(-2*bound); // Subtract when we're above.
  __m128 negBoundAdjust = _mm_set_ps1(2*bound); // Add when we're below.
  // Do the first 4 by hand since we're going in from the saveValue:
  *outPtr = *inPtr - *saveValue;
  if (*outPtr >  bound) *outPtr -= 2*bound;
  if (*outPtr < -bound) *outPtr += 2*bound;
  inPtr++;
  outPtr++;
  for (j = 1; j < ( (4 < num_points) ? 4 : num_points); j++) {
    *outPtr = *(inPtr) - *(inPtr-1);
    if (*outPtr >  bound) *outPtr -= 2*bound;
    if (*outPtr < -bound) *outPtr += 2*bound;
    inPtr++;
    outPtr++;
  }
    
  for (; number < quarterPoints; number++) {
    // Load data
    next3old1 = _mm_loadu_ps((float*) (inPtr-1));
    next4 = _mm_load_ps(inPtr);
    inPtr += 4;
    // Subtract and store:
    next3old1 = _mm_sub_ps(next4, next3old1);
    // Bound:
    boundAdjust = _mm_cmpgt_ps(next3old1, upperBound);
    boundAdjust = _mm_and_ps(boundAdjust, posBoundAdjust);
    next4 = _mm_cmplt_ps(next3old1, lowerBound);
    next4 = _mm_and_ps(next4, negBoundAdjust);
    boundAdjust = _mm_or_ps(next4, boundAdjust);
    // Make sure we're in the bounding interval:
    next3old1 = _mm_add_ps(next3old1, boundAdjust);
    _mm_store_ps(outPtr,next3old1); // Store the results back into the output
    outPtr += 4;
  }
    
  for (number = (4 > (quarterPoints*4) ? 4 : (4 * quarterPoints)); number < num_points; number++) {
    *outPtr = *(inPtr) - *(inPtr-1);
    if (*outPtr >  bound) *outPtr -= 2*bound;
    if (*outPtr < -bound) *outPtr += 2*bound;
    inPtr++;
    outPtr++;
  }
    
  *saveValue = inputVector[num_points-1];
}
#endif /* LV_HAVE_SSE */

#ifdef LV_HAVE_GENERIC
/*!
  \brief performs the FM-detect differentiation on the input vector and stores the results in the output vector.
  \param outputVector The byte-aligned vector where the results will be stored.
  \param inputVector The byte-aligned input vector containing phase data (must be on the interval (-bound,bound] )
  \param bound The interval that the input phase data is in, which is used to modulo the differentiation
  \param saveValue A pointer to a float which contains the phase value of the sample before the first input sample.
  \param num_points The number of real values in the input vector.
*/
static inline void volk_32f_s32f_32f_fm_detect_32f_a_generic(float* outputVector, const float* inputVector, const float bound, float* saveValue, unsigned int num_points){
  if (num_points < 1) {
    return;
  }
  unsigned int number = 0;
  float* outPtr = outputVector;
  const float* inPtr = inputVector;
      
  // Do the first 1 by hand since we're going in from the saveValue:
  *outPtr = *inPtr - *saveValue;
  if (*outPtr >  bound) *outPtr -= 2*bound;
  if (*outPtr < -bound) *outPtr += 2*bound;
  inPtr++;
  outPtr++;
    
  for (number = 1; number < num_points; number++) {
    *outPtr = *(inPtr) - *(inPtr-1);
    if (*outPtr >  bound) *outPtr -= 2*bound;
    if (*outPtr < -bound) *outPtr += 2*bound;
    inPtr++;
    outPtr++;
  }
    
  *saveValue = inputVector[num_points-1];
}
#endif /* LV_HAVE_GENERIC */




#endif /* INCLUDED_volk_32f_s32f_32f_fm_detect_32f_a_H */
