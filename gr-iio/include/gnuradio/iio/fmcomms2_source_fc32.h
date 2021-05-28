/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS2_SOURCE_FC32_H
#define INCLUDED_IIO_FMCOMMS2_SOURCE_FC32_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/iio/fmcomms2_source.h>
#include <gnuradio/sync_block.h>

#include "device_source.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific source for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_source_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<fmcomms2_source_f32c> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size);

    virtual void set_len_tag_key(const std::string& len_tag_key);
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_gain_mode(size_t chan, const std::string& mode);
    virtual void set_gain(size_t chan, double gain);
    virtual void set_quadrature(bool quadrature);
    virtual void set_rfdc(bool rfdc);
    virtual void set_bbdc(bool bbdc);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);

private:
    fmcomms2_source::sptr fmcomms2_block;

protected:
    explicit fmcomms2_source_f32c(bool rx1_en, bool rx2_en, fmcomms2_source::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_FC32_H */
