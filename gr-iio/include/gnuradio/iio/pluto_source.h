/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_PLUTO_SOURCE_H
#define INCLUDED_IIO_PLUTO_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {
/*!
 * \brief Source block for the PlutoSDR
 * \ingroup iio
 *
 */
class IIO_API pluto_source : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<pluto_source> sptr;

    static sptr make(const std::string& uri, unsigned long buffer_size);

    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_gain_mode(const std::string& mode) = 0;
    virtual void set_gain(double gain) = 0;
    virtual void set_quadrature(bool quadrature) = 0;
    virtual void set_rfdc(bool rfdc) = 0;
    virtual void set_bbdc(bool bbdc) = 0;
    // virtual void set_filter_source(const std::string& filter_source) = 0;
    // virtual void set_filter_filename(const std::string& filter_filename) = 0;
    // virtual void set_fpass(float fpass) = 0;
    // virtual void set_fstop(float fstop) = 0;
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0) = 0;
};
} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_H */
