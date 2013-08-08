#ifndef INCLUDED_volk_32fc_s32fc_rotator_32fc_a_H
#define INCLUDED_volk_32fc_s32fc_rotator_32fc_a_H


#include <volk/volk_complex.h>
#include <stdio.h>
#include <stdlib.h>
#define ROTATOR_RELOAD 512


#ifdef LV_HAVE_GENERIC

/*!
  \brief rotate input vector at fixed rate per sample from initial phase offset
  \param outVector The vector where the results will be stored
  \param inVector Vector to be rotated
  \param phase_inc rotational velocity
  \param phase initial phase offset
  \param num_points The number of values in inVector to be rotated and stored into cVector
*/


static inline void volk_32fc_s32fc_x2_rotator_32fc_generic(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, lv_32fc_t* phase, unsigned int num_points){    
    unsigned int i = 0; 
    int j = 0;    
    for(i = 0; i < (unsigned int)(num_points/ROTATOR_RELOAD); ++i) {
        for(j = 0; j < ROTATOR_RELOAD; ++j) {
            *outVector++ = *inVector++ * (*phase);
            (*phase) *= phase_inc;
        }
        (*phase) /= abs((*phase));
    }
    for(i = 0; i < num_points%ROTATOR_RELOAD; ++i) {
        *outVector++ = *inVector++ * (*phase);
        (*phase) *= phase_inc;
    }
    
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void volk_32fc_s32fc_x2_rotator_32fc_sse4_1(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, lv_32fc_t* phase, unsigned int num_points){
    lv_32fc_t* cPtr = outVector;
    const lv_32fc_t* aPtr = inVector;
    lv_32fc_t incr = 1;
    lv_32fc_t phase_Ptr[2] = {(*phase), (*phase)};
    
    unsigned int i, j = 0;

    for(i = 0; i < 2; ++i) {
        phase_Ptr[i] *= incr;
        incr *= (phase_inc);
    }

    /*printf("%f, %f\n", lv_creal(phase_Ptr[0]), lv_cimag(phase_Ptr[0]));
    printf("%f, %f\n", lv_creal(phase_Ptr[1]), lv_cimag(phase_Ptr[1]));
    printf("%f, %f\n", lv_creal(phase_Ptr[2]), lv_cimag(phase_Ptr[2]));
    printf("%f, %f\n", lv_creal(phase_Ptr[3]), lv_cimag(phase_Ptr[3]));
    printf("incr: %f, %f\n", lv_creal(incr), lv_cimag(incr));*/
    __m128 aVal, phase_Val, inc_Val, yl, yh, tmp1, tmp2, z, ylp, yhp, tmp1p, tmp2p;
    
    phase_Val = _mm_loadu_ps((float*)phase_Ptr);
    inc_Val = _mm_set_ps(lv_cimag(incr), lv_creal(incr),lv_cimag(incr), lv_creal(incr));
    
    const unsigned int halfPoints = num_points / 2;

    
    for(i = 0; i < (unsigned int)(halfPoints/ROTATOR_RELOAD); i++) {
        for(j = 0; j < ROTATOR_RELOAD; ++j) {
            
            aVal = _mm_load_ps((float*)aPtr);
            
            yl = _mm_moveldup_ps(phase_Val);
            yh = _mm_movehdup_ps(phase_Val);
            ylp = _mm_moveldup_ps(inc_Val);
            yhp = _mm_movehdup_ps(inc_Val);
            
            tmp1 = _mm_mul_ps(aVal, yl);
            tmp1p = _mm_mul_ps(phase_Val, ylp);
            
            aVal = _mm_shuffle_ps(aVal, aVal, 0xB1);
            phase_Val = _mm_shuffle_ps(phase_Val, phase_Val, 0xB1);
            tmp2 = _mm_mul_ps(aVal, yh);
            tmp2p = _mm_mul_ps(phase_Val, yhp);
            
            z = _mm_addsub_ps(tmp1, tmp2);
            phase_Val = _mm_addsub_ps(tmp1p, tmp2p);
            
            _mm_store_ps((float*)cPtr, z);
            
            aPtr += 2;
            cPtr += 2;
        }
        tmp1 = _mm_mul_ps(phase_Val, phase_Val);
        tmp2 = _mm_hadd_ps(tmp1, tmp1);
        tmp1 = _mm_shuffle_ps(tmp2, tmp2, 0xD8);
        phase_Val = _mm_div_ps(phase_Val, tmp1);
    }
    for(i = 0; i < halfPoints%ROTATOR_RELOAD; ++i) {
        aVal = _mm_load_ps((float*)aPtr);
        
        yl = _mm_moveldup_ps(phase_Val);
        yh = _mm_movehdup_ps(phase_Val);
        ylp = _mm_moveldup_ps(inc_Val);
        yhp = _mm_movehdup_ps(inc_Val);
        
        tmp1 = _mm_mul_ps(aVal, yl);

        tmp1p = _mm_mul_ps(phase_Val, ylp);
        
        aVal = _mm_shuffle_ps(aVal, aVal, 0xB1);
        phase_Val = _mm_shuffle_ps(phase_Val, phase_Val, 0xB1);
        tmp2 = _mm_mul_ps(aVal, yh);
        tmp2p = _mm_mul_ps(phase_Val, yhp);
        
        z = _mm_addsub_ps(tmp1, tmp2);
        phase_Val = _mm_addsub_ps(tmp1p, tmp2p);
        
        _mm_store_ps((float*)cPtr, z);
        
        aPtr += 2;
        cPtr += 2;
    }

    _mm_storeu_ps((float*)phase_Ptr, phase_Val);
    for(i = 0; i < num_points%2; ++i) {
        *cPtr++ = *aPtr++ * phase_Ptr[0];
        phase_Ptr[0] *= (phase_inc);
    }
     
    (*phase) = phase_Ptr[0];

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




static inline void volk_32fc_s32fc_x2_rotator_32fc_avx(lv_32fc_t* outVector, const lv_32fc_t* inVector, const lv_32fc_t phase_inc, lv_32fc_t* phase, unsigned int num_points){
    lv_32fc_t* cPtr = outVector;
    const lv_32fc_t* aPtr = inVector;
    lv_32fc_t incr = 1;
    lv_32fc_t phase_Ptr[4] = {(*phase), (*phase), (*phase), (*phase)};
    
    unsigned int i, j = 0;

    for(i = 0; i < 4; ++i) {
        phase_Ptr[i] *= incr;
        incr *= (phase_inc);
    }

    /*printf("%f, %f\n", lv_creal(phase_Ptr[0]), lv_cimag(phase_Ptr[0]));
    printf("%f, %f\n", lv_creal(phase_Ptr[1]), lv_cimag(phase_Ptr[1]));
    printf("%f, %f\n", lv_creal(phase_Ptr[2]), lv_cimag(phase_Ptr[2]));
    printf("%f, %f\n", lv_creal(phase_Ptr[3]), lv_cimag(phase_Ptr[3]));
    printf("incr: %f, %f\n", lv_creal(incr), lv_cimag(incr));*/
    __m256 aVal, phase_Val, inc_Val, yl, yh, tmp1, tmp2, z, ylp, yhp, tmp1p, tmp2p;
    
    phase_Val = _mm256_loadu_ps((float*)phase_Ptr);
    inc_Val = _mm256_set_ps(lv_cimag(incr), lv_creal(incr),lv_cimag(incr), lv_creal(incr),lv_cimag(incr), lv_creal(incr),lv_cimag(incr), lv_creal(incr));
    const unsigned int fourthPoints = num_points / 4;

    
    for(i = 0; i < (unsigned int)(fourthPoints/ROTATOR_RELOAD); i++) {
        for(j = 0; j < ROTATOR_RELOAD; ++j) {
            
            aVal = _mm256_load_ps((float*)aPtr);
            
            yl = _mm256_moveldup_ps(phase_Val);
            yh = _mm256_movehdup_ps(phase_Val);
            ylp = _mm256_moveldup_ps(inc_Val);
            yhp = _mm256_movehdup_ps(inc_Val);
            
            tmp1 = _mm256_mul_ps(aVal, yl);
            tmp1p = _mm256_mul_ps(phase_Val, ylp);
            
            aVal = _mm256_shuffle_ps(aVal, aVal, 0xB1);
            phase_Val = _mm256_shuffle_ps(phase_Val, phase_Val, 0xB1);
            tmp2 = _mm256_mul_ps(aVal, yh);
            tmp2p = _mm256_mul_ps(phase_Val, yhp);
            
            z = _mm256_addsub_ps(tmp1, tmp2);
            phase_Val = _mm256_addsub_ps(tmp1p, tmp2p);
            
            _mm256_store_ps((float*)cPtr, z);
            
            aPtr += 4;
            cPtr += 4;
        }
        tmp1 = _mm256_mul_ps(phase_Val, phase_Val);
        tmp2 = _mm256_hadd_ps(tmp1, tmp1);
        tmp1 = _mm256_shuffle_ps(tmp2, tmp2, 0xD8);
        phase_Val = _mm256_div_ps(phase_Val, tmp1);
    }
    for(i = 0; i < fourthPoints%ROTATOR_RELOAD; ++i) {
        aVal = _mm256_load_ps((float*)aPtr);
        
        yl = _mm256_moveldup_ps(phase_Val);
        yh = _mm256_movehdup_ps(phase_Val);
        ylp = _mm256_moveldup_ps(inc_Val);
        yhp = _mm256_movehdup_ps(inc_Val);
        
        tmp1 = _mm256_mul_ps(aVal, yl);

        tmp1p = _mm256_mul_ps(phase_Val, ylp);
        
        aVal = _mm256_shuffle_ps(aVal, aVal, 0xB1);
        phase_Val = _mm256_shuffle_ps(phase_Val, phase_Val, 0xB1);
        tmp2 = _mm256_mul_ps(aVal, yh);
        tmp2p = _mm256_mul_ps(phase_Val, yhp);
        
        z = _mm256_addsub_ps(tmp1, tmp2);
        phase_Val = _mm256_addsub_ps(tmp1p, tmp2p);
        
        _mm256_store_ps((float*)cPtr, z);
        
        aPtr += 4;
        cPtr += 4;
    }

    _mm256_storeu_ps((float*)phase_Ptr, phase_Val);
    for(i = 0; i < num_points%4; ++i) {
        *cPtr++ = *aPtr++ * phase_Ptr[0];
        phase_Ptr[0] *= (phase_inc);
    }
     
    (*phase) = phase_Ptr[0];

}
    
#endif /* LV_HAVE_AVX */








#endif /* INCLUDED_volk_32fc_s32fc_rotator_32fc_a_H */
