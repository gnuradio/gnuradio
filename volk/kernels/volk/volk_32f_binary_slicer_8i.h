#ifndef INCLUDED_volk_32f_binary_slicer_8i_H
#define INCLUDED_volk_32f_binary_slicer_8i_H


#ifdef LV_HAVE_GENERIC
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_generic(int8_t* cVector, const float* aVector,
                                  unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++) {
    if(*aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_GENERIC
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_generic_branchless(int8_t* cVector, const float* aVector,
                                             unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  for(number = 0; number < num_points; number++){
    *cPtr++ = (*aPtr++ >= 0);
  }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_a_sse2(int8_t* cVector, const float* aVector,
                                 unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  unsigned int n16points = num_points / 16;
  __m128 a0_val, a1_val, a2_val, a3_val;
  __m128 res0_f, res1_f, res2_f, res3_f;
  __m128i res0_i, res1_i, res2_i, res3_i;
  __m128 zero_val;
  zero_val = _mm_set1_ps(0.0f);

  for(number = 0; number < n16points; number++) {
    a0_val = _mm_load_ps(aPtr);
    a1_val = _mm_load_ps(aPtr+4);
    a2_val = _mm_load_ps(aPtr+8);
    a3_val = _mm_load_ps(aPtr+12);

    // compare >= 0; return float
    res0_f = _mm_cmpge_ps(a0_val, zero_val);
    res1_f = _mm_cmpge_ps(a1_val, zero_val);
    res2_f = _mm_cmpge_ps(a2_val, zero_val);
    res3_f = _mm_cmpge_ps(a3_val, zero_val);

    // convert to 32i and >> 31
    res0_i = _mm_srli_epi32(_mm_cvtps_epi32(res0_f), 31);
    res1_i = _mm_srli_epi32(_mm_cvtps_epi32(res1_f), 31);
    res2_i = _mm_srli_epi32(_mm_cvtps_epi32(res2_f), 31);
    res3_i = _mm_srli_epi32(_mm_cvtps_epi32(res3_f), 31);

    // pack into 16-bit results
    res0_i = _mm_packs_epi32(res0_i, res1_i);
    res2_i = _mm_packs_epi32(res2_i, res3_i);

    // pack into 8-bit results
    res0_i = _mm_packs_epi16(res0_i, res2_i);

    _mm_store_si128((__m128i*)cPtr, res0_i);

    cPtr += 16;
    aPtr += 16;
  }

  for(number = n16points * 16; number < num_points; number++) {
    if( *aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_SSE2 */



#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
  \brief Returns integer 1 if float input is greater than or equal to 0, 1 otherwise
  \param cVector The char (int8_t) output (either 0 or 1)
  \param aVector The float input
  \param num_points The number of values in aVector and stored into cVector
*/
static inline void
volk_32f_binary_slicer_8i_u_sse2(int8_t* cVector, const float* aVector,
                                  unsigned int num_points)
{
  int8_t* cPtr = cVector;
  const float* aPtr = aVector;
  unsigned int number = 0;

  unsigned int n16points = num_points / 16;
  __m128 a0_val, a1_val, a2_val, a3_val;
  __m128 res0_f, res1_f, res2_f, res3_f;
  __m128i res0_i, res1_i, res2_i, res3_i;
  __m128 zero_val;
  zero_val = _mm_set1_ps (0.0f);

  for(number = 0; number < n16points; number++) {
    a0_val = _mm_loadu_ps(aPtr);
    a1_val = _mm_loadu_ps(aPtr+4);
    a2_val = _mm_loadu_ps(aPtr+8);
    a3_val = _mm_loadu_ps(aPtr+12);

    // compare >= 0; return float
    res0_f = _mm_cmpge_ps(a0_val, zero_val);
    res1_f = _mm_cmpge_ps(a1_val, zero_val);
    res2_f = _mm_cmpge_ps(a2_val, zero_val);
    res3_f = _mm_cmpge_ps(a3_val, zero_val);

    // convert to 32i and >> 31
    res0_i = _mm_srli_epi32(_mm_cvtps_epi32(res0_f), 31);
    res1_i = _mm_srli_epi32(_mm_cvtps_epi32(res1_f), 31);
    res2_i = _mm_srli_epi32(_mm_cvtps_epi32(res2_f), 31);
    res3_i = _mm_srli_epi32(_mm_cvtps_epi32(res3_f), 31);

    // pack into 16-bit results
    res0_i = _mm_packs_epi32(res0_i, res1_i);
    res2_i = _mm_packs_epi32(res2_i, res3_i);

    // pack into 8-bit results
    res0_i = _mm_packs_epi16(res0_i, res2_i);

    _mm_storeu_si128((__m128i*)cPtr, res0_i);

    cPtr += 16;
    aPtr += 16;
  }

  for(number = n16points * 16; number < num_points; number++) {
    if( *aPtr++ >= 0) {
      *cPtr++ = 1;
    }
    else {
      *cPtr++ = 0;
    }
  }
}
#endif /* LV_HAVE_SSE2 */


#endif /* INCLUDED_volk_32f_binary_slicer_8i_H */
