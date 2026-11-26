/* -*- c++ -*- */
/*
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "single_pole_iir_quick_start.h"
#include <limits>

namespace gr {
namespace filter {

template <class T>
single_pole_iir_quick_start<T>::single_pole_iir_quick_start(double alpha,
                                                            bool quick_start)
    : base(alpha), d_next_sample(quick_start ? 1 : 0)
{
}

template <class T>
T single_pole_iir_quick_start<T>::filter(T input)
{
    if (d_next_sample != 0) {
        const double weight = 1.0 / static_cast<double>(d_next_sample);
        if (weight > this->d_alpha) {
            if (d_next_sample < std::numeric_limits<std::uint64_t>::max())
                ++d_next_sample;

            using state_type = decltype(this->d_prev_output);
            this->d_prev_output = weight * static_cast<state_type>(input) +
                                  (1.0 - weight) * this->d_prev_output;
            return static_cast<T>(this->d_prev_output);
        }
        d_next_sample = 0;
    }
    return base::filter(input);
}

template class single_pole_iir_quick_start<float>;
template class single_pole_iir_quick_start<gr_complex>;

} // namespace filter
} // namespace gr
