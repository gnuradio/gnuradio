/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
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

#include "ad9081_common.h"
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
class IIO_API ad9081_sink : virtual public gr::sync_block, virtual public ad9081_common
{
protected:
    ad9081_sink() = default;

public:
    typedef std::shared_ptr<ad9081_sink> sptr;

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
                     std::array<bool, MAX_CHANNEL_COUNT> en,
                     size_t buffer_size,
                     bool cyclic = false);

    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_sink
     *
     * \param ctx   An iio context to operate on
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     * \param buffer_size   The iio buffer size
     * \param cyclic    Whether the output buffers should be sent out repeatedly
     *                  until a new buffer is provided. When cyclic is set to false,
     *                  every buffer will only be used once, but there may be large
     *                  gaps between them, depending on your device sample rate and
     *                  connection.
     */
    static sptr make_from(struct iio_context* ctx,
                          std::array<bool, MAX_CHANNEL_COUNT> en,
                          size_t buffer_size,
                          bool cyclic = false);
};

/*!
 * \brief Device specific sink for AD9081 MxFE transceivers
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for AD9081-FMCA-EBZ evaluation
 * cards. However, it should support any AD9081 based device using the IIO
 * driver.
 */
class IIO_API ad9081_sink_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<ad9081_sink_f32c> sptr;

    static constexpr int MAX_CHANNEL_COUNT = ad9081_sink::MAX_CHANNEL_COUNT;

private:
    ad9081_sink::sptr d_ad9081_block;
    ad9081_sink_f32c(const std::array<bool, MAX_CHANNEL_COUNT>& en,
                     ad9081_sink::sptr src_block);

public:
    /*!
     * \brief Return a shared_ptr to a new instance of iio::ad9081_sink
     *
     * \param uri   The context uri
     * \param en    An array of MAX_CHANNEL_COUNT booleans to control the
     *              enabled channels
     * \param buffer_size   The iio buffer size
     */
    static sptr make(const std::string& uri,
                     std::array<bool, MAX_CHANNEL_COUNT> en,
                     size_t buffer_size,
                     bool cyclic = false);

    /*!
     * \brief Set main nco target frequency in Hz
     */
    void set_main_nco_freq(int nco, int64_t freq)
    {
        d_ad9081_block->set_main_nco_freq(nco, freq);
    }

    /*!
     * \brief Set main nco phase in radians
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
     * \brief Set channel nco phase in radians
     */
    void set_channel_nco_phase(int nco, float phase)
    {
        d_ad9081_block->set_channel_nco_phase(nco, phase);
    }
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SINK_H */
