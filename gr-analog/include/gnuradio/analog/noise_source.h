/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef NOISE_SOURCE_H
#define NOISE_SOURCE_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/noise_type.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief Random number source
 * \ingroup waveform_generators_blk
 *
 * \details
 * Generate random values from different distributions.
 * Currently, only Gaussian and uniform are enabled.
 */
template <class T>
class ANALOG_API noise_source : virtual public sync_block
{
public:
    // gr::analog::noise_source::sptr
    typedef std::shared_ptr<noise_source<T>> sptr;

    /*! Build a noise source
     * \param type the random distribution to use (see
     *        gnuradio/analog/noise_type.h)
     * \param ampl the standard deviation of a 1-d noise process. If
     *        this is the complex source, this parameter is split
     *        among the real and imaginary parts:
     *        <pre>(ampl/sqrt(2))x + j(ampl/sqrt(2))y</pre>
     * \param seed seed for random generators. seed = 0 initializes
     *        the random number generator with the system time.
     */
    static sptr make(noise_type_t type, float ampl, uint64_t seed = 0);

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

typedef noise_source<std::int16_t> noise_source_s;
typedef noise_source<std::int32_t> noise_source_i;
typedef noise_source<float> noise_source_f;
typedef noise_source<gr_complex> noise_source_c;

} /* namespace analog */
} /* namespace gr */

#endif /* NOISE_SOURCE_H */
