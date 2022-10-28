/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "moving_average_cpu.h"
#include "moving_average_cpu_gen.h"

#include <numeric>

namespace gr {
namespace filter {

namespace {
template <typename T>
inline void volk_add(T* out, const T* add, unsigned int num)
{
    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_add_32f(out, out, add, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        volk_32fc_x2_add_32fc(out, out, add, num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] += add[elem];
        }
    }
}


template <typename T>
inline void volk_sub(T* out, const T* sub, unsigned int num)
{

    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_subtract_32f(out, out, sub, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        // std::complex is required to be implemented by adjacent and non-padded
        // imaginary and real values, so it's safe to treat as such.
        auto fout = reinterpret_cast<T::value_type*>(out);
        auto fsub = reinterpret_cast<const T::value_type*>(sub);

        // But just to be sure, let's double-check that.
        static_assert(
            sizeof(gr_complex) == 2 * sizeof(gr_complex::value_type),
            "Can't happen: sizeof(gr_complex) != 2*sizeof(gr_complex::value_type)");

        volk_32f_x2_subtract_32f(fout, fout, fsub, 2 * num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] -= sub[elem];
        }
    }
}

template <typename T>
inline void volk_mul(T* out, const T* in, const T* scale, unsigned int num)
{
    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_multiply_32f(out, in, scale, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        volk_32fc_x2_multiply_32fc(out, in, scale, num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] = in[elem] * scale[elem];
        }
    }
}
} // namespace


template <class T>
moving_average_cpu<T>::moving_average_cpu(
    const typename moving_average<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_max_iter(args.max_iter), d_vlen(args.vlen)
{
    // this->set_history(length);
    this->declare_noconsume(args.length - 1);

    // we store this vector so that work() doesn't spend its time allocating and freeing
    // vector storage
    if (d_vlen > 1) {
        d_sum = volk::vector<T>(d_vlen);
        d_scales = volk::vector<T>(d_vlen, args.scale);
    }
}

template <class T>
work_return_t moving_average_cpu<T>::enforce_constraints(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;

    // Constrain inputs based on outputs
    if (ninput_items <= this->noconsume() || noutput_items <= this->noconsume()) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    size_t nin = std::min(ninput_items - this->noconsume(), noutput_items);

    wio.inputs()[0].n_items = nin;
    wio.outputs()[0].n_items = nin;

    return work_return_t::OK;
}

template <class T>
work_return_t moving_average_cpu<T>::work(work_io& wio)
{
    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();

    auto noutput_items = wio.outputs()[0].n_items;

    const unsigned int num_iter = std::min(noutput_items, d_max_iter);
    auto length = pmtf::get_as<size_t>(*this->param_length);
    auto scale = pmtf::get_as<T>(*this->param_scale);

    size_t nconsume = num_iter;
    if (d_vlen == 1) {
        T sum = 0;
        if (wio.inputs()[0].buf().total_read() > 0) {
            sum = std::accumulate(&in[0], &in[length - 1], T{});
            for (unsigned int i = 0; i < num_iter; i++) {
                sum += in[i + length - 1];
                out[i] = sum * scale;
                sum -= in[i];
            }
        }
        else {
            nconsume = num_iter - this->noconsume();
            for (unsigned int i = 0; i < length - 1; i++) {
                sum += in[i];
                out[i] = sum * scale;
            }
            for (unsigned int i = length - 1; i < num_iter; i++) {
                sum += in[i];
                out[i] = sum * scale;
                sum -= in[i - (length - 1)];
            }
        }
    }
    else { // d_vlen > 1
        // gets automatically optimized well
        if (wio.inputs()[0].buf().total_read() > 0) {
            std::copy(&in[0], &in[d_vlen], std::begin(d_sum));
            for (size_t i = 1; i < length - 1; i++) {
                volk_add(d_sum.data(), &in[i * d_vlen], d_vlen);
            }
            for (size_t i = 0; i < num_iter; i++) {
                volk_add(d_sum.data(), &in[(i + length - 1) * d_vlen], d_vlen);
                volk_mul(&out[i * d_vlen], d_sum.data(), d_scales.data(), d_vlen);
                volk_sub(d_sum.data(), &in[i * d_vlen], d_vlen);
            }
        }
        else {
            std::fill(d_sum.begin(), d_sum.end(), 0);
            nconsume = num_iter - this->noconsume();
            for (size_t i = 0; i < length - 1; i++) {
                volk_add(d_sum.data(), &in[(i + length - 1) * d_vlen], d_vlen);
                volk_mul(&out[i * d_vlen], d_sum.data(), d_scales.data(), d_vlen);
            }
            for (size_t i = length - 1; i < num_iter; i++) {
                volk_add(d_sum.data(), &in[(i + length - 1) * d_vlen], d_vlen);
                volk_mul(&out[i * d_vlen], d_sum.data(), d_scales.data(), d_vlen);
                volk_sub(d_sum.data(), &in[(i - (length - 1)) * d_vlen], d_vlen);
            }
        }
    }

    wio.consume_each(nconsume);
    wio.produce_each(num_iter);
    return work_return_t::OK;
}

} /* namespace filter */
} /* namespace gr */
