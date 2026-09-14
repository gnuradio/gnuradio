/* -*- c++ -*- */
/*
 * Copyright 2026 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_EVENT_MONITOR_H
#define INCLUDED_IIO_EVENT_MONITOR_H

#include <gnuradio/block.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {

/*!
 * \brief Reports hardware events (threshold/magnitude/roc/gesture/fault/...)
 *        pushed by an IIO device's event interface
 * \ingroup iio
 *
 * \details
 * This block opens an event stream on the given IIO device and, for every
 * hardware event the driver reports, publishes a PMT dict (keys "type",
 * "direction", "channel", "timestamp") on its "event" message port.
 *
 * Only available when gr-iio is built against libiio v1, which is the only
 * libiio version that provides the event-stream API.
 */
class IIO_API event_monitor : virtual public gr::block
{
public:
    typedef std::shared_ptr<event_monitor> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::event_monitor.
     *
     * \param uri     URI of the IIO context to connect to
     * \param device  Name or ID of the IIO device to monitor for events
     */
    static sptr make(const std::string& uri, const std::string& device);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_EVENT_MONITOR_H */
