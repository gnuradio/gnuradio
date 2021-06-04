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

#include <gnuradio/iio/fmcomms2_source_fc32.h>
#include <gnuradio/iio/pluto_source.h>

#include "device_source_impl.h"

namespace gr {
namespace iio {

class pluto_source_impl : public pluto_source, public fmcomms2_source_f32c
{
public:
    explicit pluto_source_impl(fmcomms2_source::sptr block);

    static std::string get_uri();

    virtual void set_len_tag_key(const std::string& len_tag_key) override;
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_gain_mode(const std::string& mode);
    virtual void set_gain(double gain);
    virtual void set_quadrature(bool quadrature);
    virtual void set_rfdc(bool rfdc);
    virtual void set_bbdc(bool bbdc);

    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_IMPL_H */
