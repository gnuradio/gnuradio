/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS2_SOURCE_H
#define INCLUDED_IIO_FMCOMMS2_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
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
class IIO_API fmcomms2_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<fmcomms2_source> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size);

    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_gain_mode(size_t chan, const std::string& mode) = 0;
    virtual void set_gain(size_t chan, double gain) = 0;
    virtual void set_quadrature(bool quadrature) = 0;
    virtual void set_rfdc(bool rfdc) = 0;
    virtual void set_bbdc(bool bbdc) = 0;
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0) = 0;
};

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
                     unsigned long buffer_size)
    {
        fmcomms2_source::sptr block = fmcomms2_source::make(uri, ch_en, buffer_size);

        return gnuradio::get_initial_sptr(new fmcomms2_source_f32c(
            (ch_en.size() > 0 && ch_en[0]), (ch_en.size() > 1 && ch_en[1]), block));
    }

    virtual void set_frequency(unsigned long long frequency)
    {
        fmcomms2_block->set_frequency(frequency);
    }
    virtual void set_samplerate(unsigned long samplerate)
    {
        fmcomms2_block->set_samplerate(samplerate);
    }
    virtual void set_gain_mode(size_t chan, const std::string& mode)
    {
        fmcomms2_block->set_gain_mode(chan, mode);
    }
    virtual void set_gain(size_t chan, double gain)
    {
        fmcomms2_block->set_gain(chan, gain);
    }
    virtual void set_quadrature(bool quadrature)
    {
        fmcomms2_block->set_quadrature(quadrature);
    }
    virtual void set_rfdc(bool rfdc) { fmcomms2_block->set_rfdc(rfdc); }
    virtual void set_bbdc(bool bbdc) { fmcomms2_block->set_bbdc(bbdc); }
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0)
    {
        fmcomms2_block->set_filter_params(filter_source, filter_filename, fpass, fstop);
    }

private:
    fmcomms2_source::sptr fmcomms2_block;

protected:
    explicit fmcomms2_source_f32c(bool rx1_en, bool rx2_en, fmcomms2_source::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_H */
