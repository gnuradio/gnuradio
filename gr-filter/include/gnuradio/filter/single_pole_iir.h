/* -*- c++ -*- */
/*
 * Copyright 2002,2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_SINGLE_POLE_IIR_H
#define INCLUDED_SINGLE_POLE_IIR_H

#include <gnuradio/filter/api.h>
#include <gnuradio/gr_complex.h>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace gr {
namespace filter {

namespace detail {

class single_pole_iir_startup
{
public:
    explicit single_pole_iir_startup(bool enabled)
        : d_enabled(enabled), d_next_sample(enabled ? 1 : 0)
    {
    }

    void reset() { d_next_sample = d_enabled ? 1 : 0; }

    template <class T>
    std::pair<T, T> taps(T alpha, T one_minus_alpha)
    {
        if (d_next_sample != 0) {
            const T startup_alpha = 1.0 / static_cast<double>(d_next_sample);
            if (startup_alpha > alpha) {
                if (d_next_sample < std::numeric_limits<std::uint64_t>::max())
                    ++d_next_sample;
                return { startup_alpha, static_cast<T>(1.0 - startup_alpha) };
            }
            d_next_sample = 0;
        }
        return { alpha, one_minus_alpha };
    }

private:
    bool d_enabled;
    std::uint64_t d_next_sample;
};

} // namespace detail

/*!
 * \brief class template for single pole IIR filter
 */
template <class o_type, class i_type, class tap_type>
class single_pole_iir
{
public:
    /*!
     * \brief construct new single pole IIR with given alpha
     *
     * computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)
     * If \p quick_start is true, use a running mean until its sample weight
     * reaches alpha.
     */
    single_pole_iir(tap_type alpha = 1.0, bool quick_start = false)
        : d_startup(quick_start)
    {
        d_prev_output = 0;
        set_taps(alpha);
    }

    /*!
     * \brief compute a single output value.
     * \returns the filtered input value.
     */
    o_type filter(const i_type input);

    /*!
     * \brief compute an array of N output values.
     * \p input must have n valid entries.
     */
    void filterN(o_type output[], const i_type input[], unsigned long n);

    /*!
     * \brief install \p alpha as the current taps.
     */
    void set_taps(tap_type alpha)
    {
        if (alpha < 0 || alpha > 1)
            throw std::out_of_range("Alpha must be in [0, 1]");

        d_alpha = alpha;
        d_one_minus_alpha = 1.0 - alpha;
    }

    //! Reset the output to zero and restart quick start if enabled.
    void reset()
    {
        d_prev_output = 0;
        d_startup.reset();
    }

    o_type prev_output() const { return d_prev_output; }

protected:
    tap_type d_alpha;
    tap_type d_one_minus_alpha;
    o_type d_prev_output;

private:
    detail::single_pole_iir_startup d_startup;
};

//
// general case.  We may want to specialize this
//
template <class o_type, class i_type, class tap_type>
o_type single_pole_iir<o_type, i_type, tap_type>::filter(const i_type input)
{
    const auto [alpha, one_minus_alpha] = d_startup.taps(d_alpha, d_one_minus_alpha);
    d_prev_output = alpha * input + one_minus_alpha * d_prev_output;
    return d_prev_output;
}


template <class o_type, class i_type, class tap_type>
void single_pole_iir<o_type, i_type, tap_type>::filterN(o_type output[],
                                                        const i_type input[],
                                                        unsigned long n)
{
    for (unsigned i = 0; i < n; i++)
        output[i] = filter(input[i]);
}


//
// Specialized case for gr_complex output and double taps
// We need to have a gr_complexd type for the calculations and prev_output variable (in
// stead of double)

template <class i_type>
class single_pole_iir<gr_complex, i_type, double>
{
public:
    /*!
     * \brief construct new single pole IIR with given alpha
     *
     * computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)
     * If \p quick_start is true, use a running mean until its sample weight
     * reaches alpha.
     */
    single_pole_iir(double alpha = 1.0, bool quick_start = false) : d_startup(quick_start)
    {
        d_prev_output = 0;
        set_taps(alpha);
    }

    /*!
     * \brief compute a single output value.
     * \returns the filtered input value.
     */
    gr_complex filter(const i_type input);

    /*!
     * \brief compute an array of N output values.
     * \p input must have n valid entries.
     */
    void filterN(gr_complex output[], const i_type input[], unsigned long n);

    /*!
     * \brief install \p alpha as the current taps.
     */
    void set_taps(double alpha)
    {
        if (alpha < 0 || alpha > 1)
            throw std::out_of_range("Alpha must be in [0, 1]");

        d_alpha = alpha;
        d_one_minus_alpha = 1.0 - alpha;
    }

    //! Reset the output to zero and restart quick start if enabled.
    void reset()
    {
        d_prev_output = 0;
        d_startup.reset();
    }

    gr_complexd prev_output() const { return d_prev_output; }

protected:
    double d_alpha;
    double d_one_minus_alpha;
    gr_complexd d_prev_output;

private:
    detail::single_pole_iir_startup d_startup;
};

template <class i_type>
gr_complex single_pole_iir<gr_complex, i_type, double>::filter(const i_type input)
{
    const auto [alpha, one_minus_alpha] = d_startup.taps(d_alpha, d_one_minus_alpha);
    d_prev_output = alpha * gr_complexd(input) + one_minus_alpha * d_prev_output;
    return gr_complex(d_prev_output);
}

// Do we need to specialize this, although it is the same as the general case?

template <class i_type>
void single_pole_iir<gr_complex, i_type, double>::filterN(gr_complex output[],
                                                          const i_type input[],
                                                          unsigned long n)
{
    for (unsigned i = 0; i < n; i++)
        output[i] = filter(input[i]);
}

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_SINGLE_POLE_IIR_H */
