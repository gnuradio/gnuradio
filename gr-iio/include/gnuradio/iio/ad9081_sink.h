/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_SINK_H
#define INCLUDED_IIO_AD9081_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_sink.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific sink for AD9081 MxFE transceivers
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for AD9081-FMCA-EBZ evaluation
 * cards. However, it should support any AD9081 based device using the IIO
 * driver.
 */
class IIO_API ad9081_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<ad9081_sink> sptr;

    static constexpr int MAX_CHANNEL_COUNT = 8;

    // private:
    //     typedef std::shared_ptr<gr::sync_block> sync_sptr;

    //     sync_sptr d_ad9081_block;
    //     ad9081_sink(const std::array<bool, MAX_CHANNEL_COUNT>& en, sync_sptr
    //     src_block);

public:
    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_sink
     *
     * \param uri   The context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     * \param buffer_size   The iio buffer size
     * \param cyclic    Whether the output buffers should be sent out repeatedly
     *                  until a new buffer is provided. When cyclic is set to false,
     *                  every buffer will only be used once, but there may be large
     *                  gaps between them, depending on your device sample rate and
     *                  connection.
     */
    static sptr make(const std::string& uri,
                     const std::array<bool, MAX_CHANNEL_COUNT>& en,
                     size_t buffer_size,
                     bool cyclic = false);

    /*!
     * \brief Set main nco target frequency in Hz
     */
    virtual void set_main_nco_freq(int nco, double freq) = 0;

    /*!
     * \brief Set main nco phase in radians
     */
    virtual void set_main_nco_phase(int nco, float phase) = 0;

    /*!
     * \brief Set channel nco frequency in Hz
     */
    virtual void set_channel_nco_freq(int nco, double freq) = 0;

    /*!
     * \brief Set channel nco phase in radians
     */
    virtual void set_channel_nco_phase(int nco, float phase) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SINK_H */
