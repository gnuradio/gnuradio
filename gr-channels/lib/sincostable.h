#ifndef SINCOSTABLE_H
#define SINCOSTABLE_H

#include <gnuradio/math.h>
#include <cmath>
#include <stdexcept>
#include <vector>

class sincostable
{
    std::vector<float> d_cos;
    size_t d_mask;
    size_t d_quarter;
    float d_scale;

    // masking also wraps negative angles
    size_t index(float x) const
    {
        return static_cast<size_t>(static_cast<long>(x * d_scale)) & d_mask;
    }

public:
    sincostable(size_t tbl_size)
        : d_cos(tbl_size, 1),
          d_mask(tbl_size - 1),
          d_quarter(tbl_size / 4),
          d_scale(tbl_size / (GR_M_PI * 2))
    {
        if (tbl_size < 4 || (tbl_size & (tbl_size - 1)) != 0) {
            throw std::invalid_argument("sincostable: size must be a power of 2");
        }
        for (size_t i = 0; i < tbl_size; i++) {
            d_cos[i] = ::cos(2 * GR_M_PI * i / tbl_size);
        }
    }
    // sin(x) = cos(x - pi/2), i.e. a quarter table back
    float sin(float x) const { return d_cos[(index(x) - d_quarter) & d_mask]; }
    float cos(float x) const { return d_cos[index(x)]; }
    float sinc(float x) const { return x == 0 ? 1 : sin(x) / x; }
};

#endif
