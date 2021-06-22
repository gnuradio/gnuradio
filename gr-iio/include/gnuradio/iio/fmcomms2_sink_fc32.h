/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS2_SINK_FC32_H
#define INCLUDED_IIO_FMCOMMS2_SINK_FC32_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/iio/fmcomms2_sink.h>
#include <gnuradio/sync_block.h>

#include "device_sink.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific sink for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_sink_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<fmcomms2_sink_f32c> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size,
                     bool cyclic);

    virtual void set_len_tag_key(const std::string& len_tag_key);
    virtual void set_bandwidth(unsigned long bandwidth);
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_attenuation(size_t chan, double attenuation);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);

private:
    fmcomms2_sink::sptr fmcomms2_block;

protected:
    explicit fmcomms2_sink_f32c(bool tx1_en, bool tx2_en, fmcomms2_sink::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_FC32_H */
