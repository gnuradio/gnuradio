/* -*- c++ -*- */
/*
 * Copyright 2026 Analog Devices Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "event_monitor_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>

#include <cerrno>
#include <stdexcept>

namespace gr {
namespace iio {

namespace {

const char* const event_type_names[] = {
    "thresh",  "mag",  "roc",   "thresh_adaptive",  "mag_adaptive",
    "change",  "mag_referenced", "gesture", "fault",
};

const char* const event_direction_names[] = {
    "either", "rising", "falling", "none", "singletap", "doubletap", "openwire",
};

std::string event_type_name(enum iio_event_type type)
{
    if ((size_t)type < sizeof(event_type_names) / sizeof(event_type_names[0]))
        return event_type_names[type];
    return "unknown(" + std::to_string((int)type) + ")";
}

std::string event_direction_name(enum iio_event_direction dir)
{
    if ((size_t)dir < sizeof(event_direction_names) / sizeof(event_direction_names[0]))
        return event_direction_names[dir];
    return "unknown(" + std::to_string((int)dir) + ")";
}

} // namespace

event_monitor::sptr event_monitor::make(const std::string& uri, const std::string& device)
{
    return gnuradio::make_block_sptr<event_monitor_impl>(uri, device);
}

event_monitor_impl::event_monitor_impl(const std::string& uri, const std::string& device)
    : gr::block("event_monitor",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(0, 0, 0)),
      d_port(pmt::mp("event")),
      d_ctx(nullptr),
      d_dev(nullptr),
      d_stream(nullptr),
      d_finished(false)
{
    d_ctx = device_source_impl::get_context(uri);
    if (!d_ctx)
        throw std::runtime_error("Unable to create IIO context");

    d_dev = iio_context_find_device(d_ctx, device.c_str());
    if (!d_dev) {
        device_source_impl::remove_ctx_history(d_ctx, true);
        d_ctx = nullptr;
        throw std::runtime_error("Device not found: " + device);
    }

    message_port_register_out(d_port);
}

event_monitor_impl::~event_monitor_impl()
{
    if (d_ctx)
        device_source_impl::remove_ctx_history(d_ctx, true);
}

bool event_monitor_impl::start()
{
    d_stream = iio_device_create_event_stream(d_dev);
    int err = iio_err(d_stream);
    if (err) {
        d_stream = nullptr;
        throw std::runtime_error("Unable to create event stream: " + std::to_string(err));
    }

    d_finished = false;
    d_thread =
        std::shared_ptr<gr::thread::thread>(new gr::thread::thread([this] { run(); }));

    return block::start();
}

bool event_monitor_impl::stop()
{
    d_finished = true;

    if (d_stream) {
        iio_event_stream_destroy(d_stream);
        d_stream = nullptr;
    }

    if (d_thread)
        d_thread->join();

    return block::stop();
}

void event_monitor_impl::run()
{
    struct iio_event event;

    while (!d_finished) {
        int ret = iio_event_stream_read(d_stream, &event, false);
        if (ret == -EINTR)
            break;

        if (ret < 0) {
            if (!d_finished) {
                char buf[256];
                iio_strerror(-ret, buf, sizeof(buf));
                d_logger->warn("Unable to read event: {:s}", buf);
            }
            break;
        }

        enum iio_event_type type = iio_event_get_type(&event);
        enum iio_event_direction dir = iio_event_get_direction(&event);
        const iio_channel* chn = iio_event_get_channel(&event, d_dev, false);

        pmt::pmt_t msg = pmt::make_dict();
        msg = pmt::dict_add(
            msg, pmt::mp("type"), pmt::string_to_symbol(event_type_name(type)));
        msg = pmt::dict_add(msg, pmt::mp("type_id"), pmt::from_long((long)type));
        msg = pmt::dict_add(
            msg, pmt::mp("direction"), pmt::string_to_symbol(event_direction_name(dir)));
        msg = pmt::dict_add(msg, pmt::mp("direction_id"), pmt::from_long((long)dir));
        msg = pmt::dict_add(msg,
                            pmt::mp("channel"),
                            pmt::string_to_symbol(chn ? iio_channel_get_id(chn) : ""));
        msg = pmt::dict_add(
            msg, pmt::mp("timestamp"), pmt::from_uint64((uint64_t)event.timestamp));

        message_port_pub(d_port, msg);
    }
}

} // namespace iio
} // namespace gr
