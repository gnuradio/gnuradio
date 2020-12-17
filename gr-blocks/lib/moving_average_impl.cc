/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2013,2017,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "moving_average_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <numeric>

namespace gr {
namespace blocks {

namespace {
template <typename T>
inline void volk_add(T* out, const T* add, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++) {
        out[elem] += add[elem];
    }
}

template <>
inline void volk_add<float>(float* out, const float* add, unsigned int num)
{
    volk_32f_x2_add_32f(out, out, add, num);
}

template <>
inline void volk_add<gr_complex>(gr_complex* out, const gr_complex* add, unsigned int num)
{
    volk_32fc_x2_add_32fc(out, out, add, num);
}

template <typename T>
inline void volk_sub(T* out, const T* sub, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++) {
        out[elem] -= sub[elem];
    }
}

template <>
inline void volk_sub<float>(float* out, const float* sub, unsigned int num)
{
    volk_32f_x2_subtract_32f(out, out, sub, num);
}

// For gr_complex, do some checking that the types are as we expect before
// allowing the specialization.
// That long `enable_if` boils down to the word `void`.
template <>
inline typename std::enable_if<std::is_same<gr_complex::value_type, float>::value>::type
volk_sub<gr_complex>(gr_complex* out, const gr_complex* sub, unsigned int num)
{
    // std::complex is required to be implemented by adjacent and non-padded
    // imaginary and real values, so it's safe to treat as such.
    auto fout = reinterpret_cast<gr_complex::value_type*>(out);
    auto fsub = reinterpret_cast<const gr_complex::value_type*>(sub);

    // But just to be sure, let's double-check that.
    static_assert(sizeof(gr_complex) == 2 * sizeof(gr_complex::value_type),
                  "Can't happen: sizeof(gr_complex) != 2*sizeof(gr_complex::value_type)");

    volk_32f_x2_subtract_32f(fout, fout, fsub, 2 * num);
}

template <typename T>
inline void volk_mul(T* out, const T* in, const T* scale, unsigned int num)
{
    for (unsigned int elem = 0; elem < num; elem++) {
        out[elem] = in[elem] * scale[elem];
    }
}

template <>
inline void
volk_mul<float>(float* out, const float* in, const float* mul, unsigned int num)
{
    volk_32f_x2_multiply_32f(out, in, mul, num);
}

template <>
inline void volk_mul<gr_complex>(gr_complex* out,
                                 const gr_complex* in,
                                 const gr_complex* mul,
                                 unsigned int num)
{
    volk_32fc_x2_multiply_32fc(out, in, mul, num);
}
} // namespace

template <class T>
typename moving_average<T>::sptr
moving_average<T>::make(int length, T scale, int max_iter, unsigned int vlen)
{
    return gnuradio::make_block_sptr<moving_average_impl<T>>(
        length, scale, max_iter, vlen);
}

template <class T>
moving_average_impl<T>::moving_average_impl(int length,
                                            T scale,
                                            int max_iter,
                                            unsigned int vlen)
    : sync_block("moving_average",
                 io_signature::make(1, 1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_length(length),
      d_scale(scale),
      d_max_iter(max_iter),
      d_vlen(vlen),
      d_new_length(length),
      d_new_scale(scale),
      d_updated(false)
{
    this->set_history(length);
    // we store this vector so that work() doesn't spend its time allocating and freeing
    // vector storage
    if (d_vlen > 1) {
        d_sum = volk::vector<T>(d_vlen);
        d_scales = volk::vector<T>(d_vlen, d_scale);
    }
}

template <class T>
moving_average_impl<T>::~moving_average_impl()
{
}

template <class T>
void moving_average_impl<T>::set_length_and_scale(int length, T scale)
{
    d_new_length = length;
    d_new_scale = scale;
    d_updated = true;
}

template <class T>
void moving_average_impl<T>::set_length(int length)
{
    d_new_length = length;
    d_updated = true;
}

template <class T>
void moving_average_impl<T>::set_scale(T scale)
{
    d_new_scale = scale;
    d_updated = true;
}

template <class T>
int moving_average_impl<T>::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    if (d_updated) {
        d_length = d_new_length;
        d_scale = d_new_scale;
        std::fill(std::begin(d_scales), std::end(d_scales), d_scale);
        this->set_history(d_length);
        d_updated = false;
        return 0; // history requirements might have changed
    }

    const T* in = static_cast<const T*>(input_items[0]);
    T* out = static_cast<T*>(output_items[0]);

    const unsigned int num_iter = std::min(noutput_items, d_max_iter);
    if (d_vlen == 1) {
        T sum = std::accumulate(&in[0], &in[d_length - 1], T{});

        for (unsigned int i = 0; i < num_iter; i++) {
            sum += in[i + d_length - 1];
            out[i] = sum * d_scale;
            sum -= in[i];
        }

    } else { // d_vlen > 1
        // gets automatically optimized well
        std::copy(&in[0], &in[d_vlen], std::begin(d_sum));

        for (int i = 1; i < d_length - 1; i++) {
            volk_add(d_sum.data(), &in[i * d_vlen], d_vlen);
        }

        for (unsigned int i = 0; i < num_iter; i++) {
            volk_add(d_sum.data(), &in[(i + d_length - 1) * d_vlen], d_vlen);
            volk_mul(&out[i * d_vlen], d_sum.data(), d_scales.data(), d_vlen);
            volk_sub(d_sum.data(), &in[i * d_vlen], d_vlen);
        }
    }
    return num_iter;
}

template class moving_average<std::int16_t>;
template class moving_average<std::int32_t>;
template class moving_average<float>;
template class moving_average<gr_complex>;
} /* namespace blocks */
} /* namespace gr */
