/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS2_SINK_H
#define INCLUDED_IIO_FMCOMMS2_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
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
class IIO_API fmcomms2_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<fmcomms2_sink> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size,
                     bool cyclic);

    virtual void set_bandwidth(unsigned long bandwidth) = 0;
    virtual void set_rf_port_select(const std::string& rf_port_select) = 0;
    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_attenuation(size_t chan, double gain) = 0;
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0) = 0;
};

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
                     bool cyclic)
    {
        fmcomms2_sink::sptr block = fmcomms2_sink::make(uri, ch_en, buffer_size, cyclic);

        return gnuradio::get_initial_sptr(new fmcomms2_sink_f32c(
            (ch_en.size() > 0 && ch_en[0]), (ch_en.size() > 1 && ch_en[1]), block));
    }

    virtual void set_bandwidth(unsigned long bandwidth)
    {
        fmcomms2_block->set_bandwidth(bandwidth);
    }
    virtual void set_frequency(unsigned long long frequency)
    {
        fmcomms2_block->set_frequency(frequency);
    }
    virtual void set_samplerate(unsigned long samplerate)
    {
        fmcomms2_block->set_samplerate(samplerate);
    }
    virtual void set_attenuation(size_t chan, double attenuation)
    {
        fmcomms2_block->set_attenuation(chan, attenuation);
    }
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0)
    {
        fmcomms2_block->set_filter_params(filter_source, filter_filename, fpass, fstop);
    }


private:
    fmcomms2_sink::sptr fmcomms2_block;

protected:
    explicit fmcomms2_sink_f32c(bool tx1_en, bool tx2_en, fmcomms2_sink::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_H */
