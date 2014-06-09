#ifndef INCLUDED_volk_32f_log2_32f_a_H
#define INCLUDED_volk_32f_log2_32f_a_H


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>


#define POLY0(x, c0) _mm_set1_ps(c0)
#define POLY1(x, c0, c1) _mm_add_ps(_mm_mul_ps(POLY0(x, c1), x), _mm_set1_ps(c0))
#define POLY2(x, c0, c1, c2) _mm_add_ps(_mm_mul_ps(POLY1(x, c1, c2), x), _mm_set1_ps(c0))
#define POLY3(x, c0, c1, c2, c3) _mm_add_ps(_mm_mul_ps(POLY2(x, c1, c2, c3), x), _mm_set1_ps(c0))
#define POLY4(x, c0, c1, c2, c3, c4) _mm_add_ps(_mm_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm_set1_ps(c0))
#define POLY5(x, c0, c1, c2, c3, c4, c5) _mm_add_ps(_mm_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm_set1_ps(c0))

#define LOG_POLY_DEGREE 3

#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes base 2 log of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_log2_32f_generic(float* bVector, const float* aVector, unsigned int num_points){    
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *bPtr++ = log2(*aPtr++);
    }
 
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes base 2 log of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_log2_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

	float* bPtr = bVector;
	const float* aPtr = aVector;
    
	unsigned int number = 0;
        const unsigned int quarterPoints = num_points / 4;

	__m128 aVal, bVal, mantissa, frac, leadingOne;
	__m128i bias, exp;

	for(;number < quarterPoints; number++){    

	aVal = _mm_load_ps(aPtr); 
	bias = _mm_set1_epi32(127);
	leadingOne = _mm_set1_ps(1.0f);
	exp = _mm_sub_epi32(_mm_srli_epi32(_mm_and_si128(_mm_castps_si128(aVal), _mm_set1_epi32(0x7f800000)), 23), bias);
	bVal = _mm_cvtepi32_ps(exp);

	// Now to extract mantissa
	frac = _mm_or_ps(leadingOne, _mm_and_ps(aVal, _mm_castsi128_ps(_mm_set1_epi32(0x7fffff))));

	// This is where MIT Licensed code starts
	#if LOG_POLY_DEGREE == 6
	  mantissa = POLY5( frac, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
	#elif LOG_POLY_DEGREE == 5
	  mantissa = POLY4( frac, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
	#elif LOG_POLY_DEGREE == 4
	  mantissa = POLY3( frac, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
	#elif LOG_POLY_DEGREE == 3
	  mantissa = POLY2( frac, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
	//#else
	//#error
	#endif

	bVal = _mm_add_ps(bVal, _mm_mul_ps(mantissa, _mm_sub_ps(frac, leadingOne)));
	_mm_store_ps(bPtr, bVal);
 

	aPtr += 4;
	bPtr += 4;
	}
 
	number = quarterPoints * 4;
	for(;number < num_points; number++){
	   *bPtr++ = log2(*aPtr++);
	}
}

#endif /* LV_HAVE_SSE4_1 for aligned */

#endif /* INCLUDED_volk_32f_log2_32f_a_H */

#ifndef INCLUDED_volk_32f_log2_32f_u_H
#define INCLUDED_volk_32f_log2_32f_u_H


#ifdef LV_HAVE_GENERIC
/*!
  \brief Computes base 2 log of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_log2_32f_u_generic(float* bVector, const float* aVector, unsigned int num_points){    
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for(number = 0; number < num_points; number++){
      *bPtr++ = log2(*aPtr++);
    }
 
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>
/*!
  \brief Computes base 2 log of input vector and stores results in output vector
  \param bVector The vector where results will be stored
  \param aVector The input vector of floats
  \param num_points Number of points for which log is to be computed
*/
static inline void volk_32f_log2_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points){

	float* bPtr = bVector;
	const float* aPtr = aVector;
    
	unsigned int number = 0;
        const unsigned int quarterPoints = num_points / 4;

	__m128 aVal, bVal, mantissa, frac, leadingOne;
	__m128i bias, exp;

	for(;number < quarterPoints; number++){    

	aVal = _mm_loadu_ps(aPtr); 
	bias = _mm_set1_epi32(127);
	leadingOne = _mm_set1_ps(1.0f);
	exp = _mm_sub_epi32(_mm_srli_epi32(_mm_and_si128(_mm_castps_si128(aVal), _mm_set1_epi32(0x7f800000)), 23), bias);
	bVal = _mm_cvtepi32_ps(exp);

	// Now to extract mantissa
	frac = _mm_or_ps(leadingOne, _mm_and_ps(aVal, _mm_castsi128_ps(_mm_set1_epi32(0x7fffff))));

	// This is where MIT Licensed code starts
	#if LOG_POLY_DEGREE == 6
	  mantissa = POLY5( frac, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
	#elif LOG_POLY_DEGREE == 5
	  mantissa = POLY4( frac, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
	#elif LOG_POLY_DEGREE == 4
	  mantissa = POLY3( frac, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
	#elif LOG_POLY_DEGREE == 3
	  mantissa = POLY2( frac, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
	//#else
	//#error
	#endif

	bVal = _mm_add_ps(bVal, _mm_mul_ps(mantissa, _mm_sub_ps(frac, leadingOne)));
	_mm_storeu_ps(bPtr, bVal);
 

	aPtr += 4;
	bPtr += 4;
	}
 
	number = quarterPoints * 4;
	for(;number < num_points; number++){
	   *bPtr++ = log2(*aPtr++);
	}
}

#endif /* LV_HAVE_SSE4_1 for unaligned */

#endif /* INCLUDED_volk_32f_log2_32f_u_H */
