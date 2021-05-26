/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_SOURCE_H
#define INCLUDED_IIO_AD9081_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "ad9081_common.h"
#include "device_source.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific source for AD9081 MxFE transceivers
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for AD9081-FMCA-EBZ evaluation
 * cards. However, it should support any AD9081 based device using the IIO
 * driver.
 */
class IIO_API ad9081_source : virtual public gr::sync_block, virtual public ad9081_common
{
protected:
    ad9081_source() = default;

public:
    typedef std::shared_ptr<ad9081_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_source
     *
     * \param uri   String of the context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     * \param buffer_size   The iio buffer size
     */
    static sptr make(const std::string& uri,
                     std::array<bool, MAX_CHANNEL_COUNT> en,
                     size_t buffer_size);

    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_source
     *
     * \param uri   String of the context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     * \param buffer_size   The iio buffer size
     */
    static sptr make_from(struct iio_context* ctx,
                          std::array<bool, MAX_CHANNEL_COUNT> en,
                          size_t buffer_size);

    /*!
     * \brief Set nyquist zone.
     */
    virtual void set_nyquist_zone(bool odd) = 0;

    /*!
     * \brief Update programmable filter
     */
    virtual void set_filter_source_file(const std::string& src_file) = 0;
};

/*!
 * \brief Device specific source for AD9081 MxFE transceivers
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for AD9081-FMCA-EBZ evaluation
 * cards. However, it should support any AD9081 based device using the IIO
 * driver.
 */
class IIO_API ad9081_source_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<ad9081_source_f32c> sptr;

    static constexpr int MAX_CHANNEL_COUNT = ad9081_source::MAX_CHANNEL_COUNT;

private:
    ad9081_source::sptr d_ad9081_block;
    ad9081_source_f32c(const std::array<bool, MAX_CHANNEL_COUNT>& en,
                       ad9081_source::sptr src_block);

public:
    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_source
     *
     * \param uri   String of the context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     */
    static sptr make(const std::string& uri,
                     std::array<bool, MAX_CHANNEL_COUNT> en,
                     size_t buffer_size);

    /*!
     * \brief Set main nco target frequency in Hz
     */
    void set_main_nco_freq(int nco, int64_t freq)
    {
        d_ad9081_block->set_main_nco_freq(nco, freq);
    }

    /*!
     * \brief Set main nco phase
     */
    void set_main_nco_phase(int nco, float phase)
    {
        d_ad9081_block->set_main_nco_phase(nco, phase);
    }

    /*!
     * \brief Set channel nco frequency in Hz
     */
    void set_channel_nco_freq(int nco, int64_t freq)
    {
        d_ad9081_block->set_channel_nco_freq(nco, freq);
    }

    /*!
     * \brief Set channel nco phase
     */
    void set_channel_nco_phase(int nco, float phase)
    {
        d_ad9081_block->set_channel_nco_phase(nco, phase);
    }

    /*!
     * \brief Set nyquist zone
     */
    void set_nyquist_zone(bool odd) { d_ad9081_block->set_nyquist_zone(odd); }

    /*!
     * \brief Update programmable filter
     */
    void set_filter_source_file(const std::string& src_file)
    {
        d_ad9081_block->set_filter_source_file(src_file);
    }
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SOURCE_H */
