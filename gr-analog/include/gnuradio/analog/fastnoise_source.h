/* -*- c++ -*- */
/*
 * Copyright 2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef FASTNOISE_SOURCE_H
#define FASTNOISE_SOURCE_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/noise_type.h>
#include <gnuradio/sync_block.h>
#include <cstdint>
#include <vector>

namespace gr {
namespace analog {

/*!
 * \brief Random number source
 * \ingroup source_blk
 *
 * \details
 * Generate random values from different distributions.
 * Currently, only Gaussian and uniform are enabled.
 */
template <class T>
class ANALOG_API fastnoise_source : virtual public sync_block
{
public:
    // gr::analog::fastnoise_source::sptr
    typedef std::shared_ptr<fastnoise_source<T>> sptr;

    /*! \brief Make a fast noise source
     * \param type the random distribution to use (see
     *        gnuradio/analog/noise_type.h)
     * \param ampl the standard deviation of a 1-d noise process. If
     *        this is the complex source, this parameter is split
     *        among the real and imaginary parts:
     *        <pre>(ampl/sqrt(2))x + j(ampl/sqrt(2))y</pre>
     * \param seed seed for random generators. Note that for uniform
     *        and Gaussian distributions, this should be a negative
     *        number.
     * \param samples Number of samples to pre-generate. For performance
     *        reasons, prefer a power of 2.
     */
    static sptr
    make(noise_type_t type, float ampl, uint64_t seed = 0, size_t samples = 1024 * 16);
    virtual T sample() = 0;
    virtual T sample_unbiased() = 0;
    virtual const std::vector<T>& samples() const = 0;

    /*!
     * Set the noise type. Nominally from the
     * gr::analog::noise_type_t selections, but only GR_GAUSSIAN and
     * GR_UNIFORM are currently available.
     */
    virtual void set_type(noise_type_t type) = 0;

    /*!
     * Set the standard deviation (amplitude) of the 1-d noise
     * process.
     */
    virtual void set_amplitude(float ampl) = 0;

    virtual noise_type_t type() const = 0;
    virtual float amplitude() const = 0;
};

typedef fastnoise_source<float> fastnoise_source_f;
typedef fastnoise_source<std::int32_t> fastnoise_source_i;
typedef fastnoise_source<std::int16_t> fastnoise_source_s;
typedef fastnoise_source<gr_complex> fastnoise_source_c;
} /* namespace analog */
} /* namespace gr */


#endif /* FASTNOISE_SOURCE_H */
