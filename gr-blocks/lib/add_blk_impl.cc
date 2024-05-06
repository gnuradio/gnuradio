/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "add_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

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
} // namespace


template <class T>
typename add_blk<T>::sptr add_blk<T>::make(size_t vlen)
{
    return gnuradio::make_block_sptr<add_blk_impl<T>>(vlen);
}


template <>
add_blk_impl<float>::add_blk_impl(size_t vlen)
    : sync_block("add_ff",
                 io_signature::make(1, -1, sizeof(float) * vlen),
                 io_signature::make(1, 1, sizeof(float) * vlen)),
      d_vlen(vlen)
{
    set_alignment(std::max(1, int(volk_get_alignment() / sizeof(float))));
}

template <>
add_blk_impl<gr_complex>::add_blk_impl(size_t vlen)
    : sync_block("add_cc",
                 io_signature::make(1, -1, sizeof(gr_complex) * vlen),
                 io_signature::make(1, 1, sizeof(gr_complex) * vlen)),
      d_vlen(vlen)
{
    set_alignment(std::max(1, int(volk_get_alignment() / sizeof(gr_complex))));
}

template <class T>
add_blk_impl<T>::add_blk_impl(size_t vlen)
    : sync_block("add_blk",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(1, 1, sizeof(T) * vlen)),
      d_vlen(vlen)
{
}

template <class T>
int add_blk_impl<T>::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    T* out = (T*)output_items[0];
    int noi = d_vlen * noutput_items;

    memcpy(out, input_items[0], noi * sizeof(T));
    for (size_t i = 1; i < input_items.size(); i++) {
        volk_add(out, (const T*)input_items[i], noi);
    }

    return noutput_items;
}

template class add_blk<std::int16_t>;
template class add_blk<std::int32_t>;
template class add_blk<gr_complex>;
template class add_blk<float>;
} /* namespace blocks */
} /* namespace gr */
