#ifndef SINCOSTABLE_H
#define SINCOSTABLE_H

#define _USE_MATH_DEFINES
#include <math.h>

class sincostable {
    std::vector<float> d_cos;
    size_t d_sz;
    float d_scale;
  public:
    sincostable(size_t tbl_size) :
        d_cos(tbl_size,1),
        d_sz(tbl_size),
        d_scale(tbl_size/(M_PI*2))
        {
        for(size_t i=0; i<tbl_size; i++){
            d_cos[i] = ::cos(2*M_PI*i/tbl_size);
            }
        }
    const float sin(float x){
        int idx = (((int)(x*d_scale)) + d_sz - d_sz/4 )% d_sz;
        return d_cos[idx];
        }
    const float cos(float x){
        int idx = (((int)(x*d_scale)) + d_sz) % d_sz;
        return d_cos[idx];
        }
    const float sinc(float x){
        return x==0?1:sin(x)/x;
        }
};

#endif
