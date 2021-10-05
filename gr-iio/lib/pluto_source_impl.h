/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_PLUTO_SOURCE_IMPL_H
#define INCLUDED_IIO_PLUTO_SOURCE_IMPL_H

#include <string>
#include <vector>

#include "fmcomms2_source_impl.h"
#include <gnuradio/iio/pluto_source.h>

#include "device_source_impl.h"

namespace gr {
namespace iio {

class pluto_source_impl : public pluto_source, virtual public fmcomms2_source_impl<gr_complex>
{
public:
    pluto_source_impl(const std::string& uri,
                                            unsigned long buffer_size);

    static std::string get_uri();

    virtual void set_len_tag_key(const std::string& len_tag_key);
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_gain_mode(size_t chan, const std::string& mode);
    virtual void set_gain(size_t chan, double gain_value);
    virtual void set_quadrature(bool quadrature);
    virtual void set_rfdc(bool rfdc);
    virtual void set_bbdc(bool bbdc);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename,
                                   float fpass,
                                   float fstop);

};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_IMPL_H */
