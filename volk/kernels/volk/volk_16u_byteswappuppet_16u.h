#ifndef INCLUDED_volk_16u_byteswappuppet_16u_H
#define INCLUDED_volk_16u_byteswappuppet_16u_H


#include <stdint.h>
#include <volk/volk_16u_byteswap.h>
#include <string.h>

#ifdef LV_HAVE_GENERIC
static inline void volk_16u_byteswappuppet_16u_generic(uint16_t*output, uint16_t* intsToSwap, unsigned int num_points){

    volk_16u_byteswap_generic((uint16_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint16_t));

}
#endif

#ifdef LV_HAVE_NEON
static inline void volk_16u_byteswappuppet_16u_neon(uint16_t*output, uint16_t* intsToSwap, unsigned int num_points){

    volk_16u_byteswap_neon((uint16_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint16_t));

}
#endif

#ifdef LV_HAVE_NEON
static inline void volk_16u_byteswappuppet_16u_neon_table(uint16_t*output, uint16_t* intsToSwap, unsigned int num_points){

    volk_16u_byteswap_neon_table((uint16_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint16_t));

}
#endif

#ifdef LV_HAVE_SSE2
static inline void volk_16u_byteswappuppet_16u_u_sse2(uint16_t *output, uint16_t* intsToSwap, unsigned int num_points){

    volk_16u_byteswap_u_sse2((uint16_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint16_t));

}
#endif

#ifdef LV_HAVE_SSE2
static inline void volk_16u_byteswappuppet_16u_a_sse2(uint16_t *output, uint16_t* intsToSwap, unsigned int num_points){

    volk_16u_byteswap_a_sse2((uint16_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint16_t));

}
#endif


#endif
