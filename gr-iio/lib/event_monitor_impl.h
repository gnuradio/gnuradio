/* -*- c++ -*- */
/*
 * Copyright 2026 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_EVENT_MONITOR_IMPL_H
#define INCLUDED_IIO_EVENT_MONITOR_IMPL_H

#include <gnuradio/iio/event_monitor.h>
#include "device_source_impl.h"

#include <iio/iio.h>

#include <atomic>
#include <string>

namespace gr {
namespace iio {

class event_monitor_impl : public event_monitor
{
private:
    const pmt::pmt_t d_port;
    iio_context* d_ctx;
    iio_device* d_dev;
    iio_event_stream* d_stream;
    std::shared_ptr<gr::thread::thread> d_thread;
    std::atomic<bool> d_finished;

    void run();

public:
    event_monitor_impl(const std::string& uri, const std::string& device);
    ~event_monitor_impl();

    bool start() override;
    bool stop() override;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_EVENT_MONITOR_IMPL_H */
