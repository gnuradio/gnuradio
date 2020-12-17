/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_FIR_FILTER_WITH_BUFFER_H
#define INCLUDED_FILTER_FIR_FILTER_WITH_BUFFER_H

#include <gnuradio/filter/api.h>
#include <gnuradio/gr_complex.h>
#include <volk/volk_alloc.hh>
#include <vector>

namespace gr {
namespace filter {
namespace kernel {

/*!
 * \brief FIR with internal buffer for float input, float output and float taps.
 * \ingroup filter
 */
class FILTER_API fir_filter_with_buffer_fff
{
private:
    mutable volk::vector<float> d_output; // Temporary storage for one element.
    int d_align;
    int d_naligned;
    std::vector<float> d_taps;
    unsigned int d_ntaps;
    volk::vector<float> d_buffer_ptr;
    float* d_buffer; // Current position pointer into d_buffer_ptr.
    unsigned int d_idx;
    std::vector<volk::vector<float>> d_aligned_taps;

public:
    // CONSTRUCTORS

    /*!
     * \brief construct new FIR with given taps.
     *
     * Note that taps must be in forward order, e.g., coefficient 0 is
     * stored in new_taps[0], coefficient 1 is stored in
     * new_taps[1], etc.
     */
    fir_filter_with_buffer_fff(const std::vector<float>& taps);

    // Disable copy because of the raw pointer member d_buffer.
    fir_filter_with_buffer_fff(const fir_filter_with_buffer_fff&) = delete;
    fir_filter_with_buffer_fff& operator=(const fir_filter_with_buffer_fff&) = delete;
    fir_filter_with_buffer_fff(fir_filter_with_buffer_fff&&) = default;
    fir_filter_with_buffer_fff& operator=(fir_filter_with_buffer_fff&&) = default;

    // MANIPULATORS

    /*!
     * \brief compute a single output value.
     *
     * \p input is a single input value of the filter type
     *
     * \returns the filtered input value.
     */
    float filter(float input);

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
    float filter(const float input[], unsigned long dec);

    /*!
     * \brief compute an array of N output values.
     *
     * \p input must have (n - 1 + ntaps()) valid entries.
     * input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.
     */
    void filterN(float output[], const float input[], unsigned long n);

    /*!
     * \brief compute an array of N output values, decimating the input
     *
     * \p input must have (decimate * (n - 1) + ntaps()) valid entries.
     * input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to
     * compute the output values.
     */
    void filterNdec(float output[],
                    const float input[],
                    unsigned long n,
                    unsigned long decimate);

    // ACCESSORS

    /*!
     * \return number of taps in filter.
     */
    unsigned int ntaps() const { return d_ntaps; }

    /*!
     * \brief install \p new_taps as the current taps.
     */
    void set_taps(const std::vector<float>& taps);

    /*!
     * \return current taps
     */
    std::vector<float> taps() const;
};


/**************************************************************/


/*!
 * \brief FIR with internal buffer for gr_complex input, gr_complex output and gr_complex
 * taps. \ingroup filter
 */
class FILTER_API fir_filter_with_buffer_ccc
{
private:
    mutable volk::vector<gr_complex> d_output; // Temporary storage for one element.
    int d_align;
    int d_naligned;
    std::vector<gr_complex> d_taps;
    unsigned int d_ntaps;
    volk::vector<gr_complex> d_buffer_ptr;
    gr_complex* d_buffer; // Current position pointer into d_buffer_ptr.
    unsigned int d_idx;
    std::vector<volk::vector<gr_complex>> d_aligned_taps;

public:
    // CONSTRUCTORS

    /*!
     * \brief construct new FIR with given taps.
     *
     * Note that taps must be in forward order, e.g., coefficient 0 is
     * stored in new_taps[0], coefficient 1 is stored in
     * new_taps[1], etc.
     */
    fir_filter_with_buffer_ccc(const std::vector<gr_complex>& taps);

    // Disable copy because of the raw pointer member d_buffer.
    fir_filter_with_buffer_ccc(const fir_filter_with_buffer_ccc&) = delete;
    fir_filter_with_buffer_ccc& operator=(const fir_filter_with_buffer_ccc&) = delete;
    fir_filter_with_buffer_ccc(fir_filter_with_buffer_ccc&&) = default;
    fir_filter_with_buffer_ccc& operator=(fir_filter_with_buffer_ccc&&) = default;

    // MANIPULATORS

    /*!
     * \brief compute a single output value.
     *
     * \p input is a single input value of the filter type
     *
     * \returns the filtered input value.
     */
    gr_complex filter(gr_complex input);

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
    gr_complex filter(const gr_complex input[], unsigned long dec);

    /*!
     * \brief compute an array of N output values.
     *
     * \p input must have (n - 1 + ntaps()) valid entries.
     * input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.
     */
    void filterN(gr_complex output[], const gr_complex input[], unsigned long n);

    /*!
     * \brief compute an array of N output values, decimating the input
     *
     * \p input must have (decimate * (n - 1) + ntaps()) valid entries.
     * input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to
     * compute the output values.
     */
    void filterNdec(gr_complex output[],
                    const gr_complex input[],
                    unsigned long n,
                    unsigned long decimate);

    // ACCESSORS

    /*!
     * \return number of taps in filter.
     */
    unsigned int ntaps() const { return d_ntaps; }

    /*!
     * \brief install \p new_taps as the current taps.
     */
    void set_taps(const std::vector<gr_complex>& taps);

    /*!
     * \return current taps
     */
    std::vector<gr_complex> taps() const;
};


/**************************************************************/


/*!
 * \brief FIR with internal buffer for gr_complex input, gr_complex output and gr_complex
 * taps. \ingroup filter
 */
class FILTER_API fir_filter_with_buffer_ccf
{
private:
    mutable volk::vector<gr_complex> d_output; // Temporary storage for one element.
    int d_align;
    int d_naligned;
    std::vector<float> d_taps;
    unsigned int d_ntaps;
    volk::vector<gr_complex> d_buffer_ptr;
    gr_complex* d_buffer; // Current position pointer into d_buffer_ptr.
    unsigned int d_idx;
    std::vector<volk::vector<float>> d_aligned_taps;

public:
    // CONSTRUCTORS

    /*!
     * \brief construct new FIR with given taps.
     *
     * Note that taps must be in forward order, e.g., coefficient 0 is
     * stored in new_taps[0], coefficient 1 is stored in
     * new_taps[1], etc.
     */
    fir_filter_with_buffer_ccf(const std::vector<float>& taps);

    // Disable copy because of the raw pointer member d_buffer.
    fir_filter_with_buffer_ccf(const fir_filter_with_buffer_ccf&) = delete;
    fir_filter_with_buffer_ccf& operator=(const fir_filter_with_buffer_ccf&) = delete;
    fir_filter_with_buffer_ccf(fir_filter_with_buffer_ccf&&) = default;
    fir_filter_with_buffer_ccf& operator=(fir_filter_with_buffer_ccf&&) = default;

    // MANIPULATORS

    /*!
     * \brief compute a single output value.
     *
     * \p input is a single input value of the filter type
     *
     * \returns the filtered input value.
     */
    gr_complex filter(gr_complex input);

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
    gr_complex filter(const gr_complex input[], unsigned long dec);

    /*!
     * \brief compute an array of N output values.
     *
     * \p input must have (n - 1 + ntaps()) valid entries.
     * input[0] .. input[n - 1 + ntaps() - 1] are referenced to compute the output values.
     */
    void filterN(gr_complex output[], const gr_complex input[], unsigned long n);

    /*!
     * \brief compute an array of N output values, decimating the input
     *
     * \p input must have (decimate * (n - 1) + ntaps()) valid entries.
     * input[0] .. input[decimate * (n - 1) + ntaps() - 1] are referenced to
     * compute the output values.
     */
    void filterNdec(gr_complex output[],
                    const gr_complex input[],
                    unsigned long n,
                    unsigned long decimate);

    // ACCESSORS

    /*!
     * \return number of taps in filter.
     */
    unsigned int ntaps() const { return d_ntaps; }

    /*!
     * \brief install \p new_taps as the current taps.
     */
    void set_taps(const std::vector<float>& taps);

    /*!
     * \return current taps
     */
    std::vector<float> taps() const;
};


} /* namespace kernel */
} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_FIR_FILTER_WITH_BUFFER_H */
