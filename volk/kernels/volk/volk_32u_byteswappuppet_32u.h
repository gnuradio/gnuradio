#ifndef INCLUDED_volk_32u_byteswappuppet_32u_H
#define INCLUDED_volk_32u_byteswappuppet_32u_H


#include <stdint.h>
#include <volk/volk_32u_byteswap.h>

#ifdef LV_HAVE_GENERIC
static inline void volk_32u_byteswappuppet_32u_generic(uint32_t*output, uint32_t* intsToSwap, unsigned int num_points){

    volk_32u_byteswap_generic((uint32_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint32_t));

}
#endif

#ifdef LV_HAVE_NEON
static inline void volk_32u_byteswappuppet_32u_neon(uint32_t*output, uint32_t* intsToSwap, unsigned int num_points){

    volk_32u_byteswap_neon((uint32_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint32_t));

}
#endif

#ifdef LV_HAVE_SSE2
static inline void volk_32u_byteswappuppet_32u_u_sse2(uint32_t *output, uint32_t* intsToSwap, unsigned int num_points){

    volk_32u_byteswap_u_sse2((uint32_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint32_t));

}
#endif

#ifdef LV_HAVE_SSE2
static inline void volk_32u_byteswappuppet_32u_a_sse2(uint32_t* output, uint32_t* intsToSwap, unsigned int num_points){

    volk_32u_byteswap_a_sse2((uint32_t*)intsToSwap, num_points);
    memcpy((void*)output, (void*)intsToSwap, num_points * sizeof(uint32_t));

}
#endif


#endif
