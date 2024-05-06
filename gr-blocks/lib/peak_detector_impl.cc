/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "peak_detector_impl.h"
#include <gnuradio/io_signature.h>
#include <type_traits>
#include <cstring>
#include <limits>

namespace gr {
namespace blocks {

namespace {

// lowest_value() returns -infinity if the type has a concept of
// infinity. Otherwise it returns the lowest possible value.
//
// Positive infinity is guaranteed by std::numeric_limits<T>::has_infinity, but
// since we want negative infinity let's use IEEE754.
template <typename T,
          typename std::enable_if<std::numeric_limits<T>::is_iec559, int>::type = 0>
constexpr T lowest_value() noexcept
{
    return -std::numeric_limits<T>::infinity();
}

template <typename T,
          typename std::enable_if<!std::numeric_limits<T>::is_iec559, int>::type = 0>
constexpr T lowest_value() noexcept
{
    return std::numeric_limits<T>::lowest();
}
} // namespace

template <class T>
typename peak_detector<T>::sptr peak_detector<T>::make(float threshold_factor_rise,
                                                       float threshold_factor_fall,
                                                       int look_ahead,
                                                       float alpha)
{
    return gnuradio::make_block_sptr<peak_detector_impl<T>>(
        threshold_factor_rise, threshold_factor_fall, look_ahead, alpha);
}

template <class T>
peak_detector_impl<T>::peak_detector_impl(float threshold_factor_rise,
                                          float threshold_factor_fall,
                                          int look_ahead,
                                          float alpha)
    : sync_block("peak_detector",
                 io_signature::make(1, 1, sizeof(T)),
                 io_signature::make(1, 1, sizeof(char))),
      d_threshold_factor_rise(threshold_factor_rise),
      d_threshold_factor_fall(threshold_factor_fall),
      d_look_ahead(look_ahead),
      d_avg_alpha(alpha),
      d_avg(0)
{
}

template <class T>
peak_detector_impl<T>::~peak_detector_impl()
{
}

template <class T>
int peak_detector_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const T* iptr = (const T*)input_items[0];
    char* optr = (char*)output_items[0];

    memset(optr, 0, noutput_items * sizeof(char));

    T peak_val = lowest_value<T>();
    int peak_ind = 0;
    unsigned char state = 0;
    int i = 0;

    // printf("noutput_items %d\n",noutput_items);
    while (i < noutput_items) {
        if (state == 0) { // below threshold
            if (iptr[i] > d_avg * d_threshold_factor_rise) {
                state = 1;
            } else {
                d_avg = (d_avg_alpha)*iptr[i] + (1 - d_avg_alpha) * d_avg;
                i++;
            }
        } else if (state == 1) { // above threshold, have not found peak
            // printf("Entered State 1: %f  i: %d  noutput_items: %d\n", iptr[i], i,
            // noutput_items);
            if (iptr[i] > peak_val) {
                peak_val = iptr[i];
                peak_ind = i;
                d_avg = (d_avg_alpha)*iptr[i] + (1 - d_avg_alpha) * d_avg;
                i++;
            } else if (iptr[i] > d_avg * d_threshold_factor_fall) {
                d_avg = (d_avg_alpha)*iptr[i] + (1 - d_avg_alpha) * d_avg;
                i++;
            } else {
                optr[peak_ind] = 1;
                state = 0;
                peak_val = lowest_value<T>();
                // printf("Leaving  State 1: Peak: %f  Peak Ind: %d   i: %d noutput_items:
                // %d\n", peak_val, peak_ind, i, noutput_items);
            }
        }
    }

    if (state == 0) {
        // printf("Leave in State 0, produced %d\n",noutput_items);
        return noutput_items;
    } else { // only return up to passing the threshold
        // printf("Leave in State 1, only produced %d of %d\n",peak_ind,noutput_items);
        return peak_ind + 1;
    }
}

template class peak_detector<float>;
template class peak_detector<std::int16_t>;
template class peak_detector<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */
