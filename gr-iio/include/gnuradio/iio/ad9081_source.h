/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
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
class IIO_API ad9081_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<ad9081_source> sptr;

    static constexpr int MAX_CHANNEL_COUNT = 8;

public:
    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_source
     *
     * \param uri   String of the context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     */
    static sptr make(const std::string& uri,
                     const std::array<bool, MAX_CHANNEL_COUNT>& en,
                     size_t buffer_size);

    /*!
     * \brief Set main nco target frequency in Hz
     */
    virtual void set_main_nco_freq(int nco, double freq) = 0;

    /*!
     * \brief Set main nco phase
     */
    virtual void set_main_nco_phase(int nco, float phase) = 0;

    /*!
     * \brief Set channel nco frequency in Hz
     */
    virtual void set_channel_nco_freq(int nco, double freq) = 0;

    /*!
     * \brief Set channel nco phase
     */
    virtual void set_channel_nco_phase(int nco, float phase) = 0;

    /*!
     * \brief Set nyquist zone
     */
    virtual void set_nyquist_zone(bool odd) = 0;

    /*!
     * \brief Update programmable filter
     */
    virtual void set_filter_source_file(const std::string& src_file) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SOURCE_H */
