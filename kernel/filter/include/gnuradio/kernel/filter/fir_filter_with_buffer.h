/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/gr_complex.h>
#include <volk/volk_alloc.hh>
#include <vector>

namespace gr {
namespace kernel {
namespace filter {

/*!
 * \brief FIR with internal buffer for type T input, T output and TAP_T taps.
 * \ingroup filter
 */
template <typename T, typename TAP_T>
class fir_filter_with_buffer
{
private:
    mutable volk::vector<T> d_output; // Temporary storage for one element.
    int d_align;
    int d_naligned;
    std::vector<TAP_T> d_taps;
    unsigned int d_ntaps;
    volk::vector<T> d_buffer_ptr;
    T* d_buffer; // Current position pointer into d_buffer_ptr.
    unsigned int d_idx;
    std::vector<volk::vector<TAP_T>> d_aligned_taps;

public:
    // CONSTRUCTORS

    /*!
     * \brief construct new FIR with given taps.
     *
     * Note that taps must be in forward order, e.g., coefficient 0 is
     * stored in new_taps[0], coefficient 1 is stored in
     * new_taps[1], etc.
     */
    fir_filter_with_buffer(const std::vector<TAP_T>& taps);

    // Disable copy because of the raw pointer member d_buffer.
    fir_filter_with_buffer(const fir_filter_with_buffer&) = delete;
    fir_filter_with_buffer& operator=(const fir_filter_with_buffer&) = delete;
    fir_filter_with_buffer(fir_filter_with_buffer&&) = default;
    fir_filter_with_buffer& operator=(fir_filter_with_buffer&&) = default;

    // MANIPULATORS

    /*!
     * \brief compute a single output value.
     *
     * \p input is a single input value of the filter type
     *
     * \returns the filtered input value.
     */
    T filter(T input);

    /*!
     * \brief compute a single output value; designed for decimating filters.
     *
     * \p input is a single input value of the filter type. The value of dec is the
     *    decimating value of the filter, so input[] must have dec valid values.
     *    The filter pushes dec number of items onto the circ. buffer before computing
     *    a single output.
     *
     * \returns the filtered input value.
     */
    T filter(const T input[], size_t dec);

    /*!
     * \brief compute an array of N output values.
     *
     * \p input must have (n - 1 + ntaps()) valid entries.
     * input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.
     */
    void filterN(T output[], const T input[], size_t n);

    /*!
     * \brief compute an array of N output values, decimating the input
     *
     * \p input must have (decimate * (n - 1) + ntaps()) valid entries.
     * input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to
     * compute the output values.
     */
    void filterNdec(T output[], const T input[], size_t n, size_t decimate);

    // ACCESSORS

    /*!
     * \return number of taps in filter.
     */
    size_t ntaps() const { return d_ntaps; }

    /*!
     * \brief install \p new_taps as the current taps.
     */
    void set_taps(const std::vector<TAP_T>& taps);

    /*!
     * \return current taps
     */
    std::vector<TAP_T> taps() const;
};

typedef fir_filter_with_buffer<float, float> fir_filter_with_buffer_fff;
typedef fir_filter_with_buffer<gr_complex, float> fir_filter_with_buffer_ccf;
typedef fir_filter_with_buffer<gr_complex, gr_complex> fir_filter_with_buffer_ccc;

} // namespace filter
} // namespace kernel
} // namespace gr
