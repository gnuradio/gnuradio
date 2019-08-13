/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "attr_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <iostream>

namespace gr {
namespace iio {

attr_sink::sptr attr_sink::make(const std::string& uri,
                                const std::string& device,
                                const std::string& channel,
                                int type,
                                bool output,
                                bool required_enable)
{
    return gnuradio::get_initial_sptr(
        new attr_sink_impl(uri, device, channel, type, output, required_enable));
}

/*
 * The private constructor
 */
attr_sink_impl::attr_sink_impl(const std::string& uri,
                               const std::string& device,
                               const std::string& channel,
                               int type,
                               bool output,
                               bool required_enable)
    : gr::block(
          "attr_sink", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      device(device),
      channel(channel),
      uri(uri),
      type(type),
      output(output),
      required_enable(required_enable)
{

    ctx = device_source_impl::get_context(uri);
    if (!ctx)
        throw std::runtime_error("Unable to create context");

    dev = iio_context_find_device(ctx, device.c_str());
    if (!dev) {
        iio_context_destroy(ctx);
        throw std::runtime_error("Device not found");
    }

    if (type == 0) {
        chan = iio_device_find_channel(dev, channel.c_str(), output);
        if (!chan) {
            iio_context_destroy(ctx);
            throw std::runtime_error("Channel not found");
        }
    }

    // Required for MathWorks generated IP
    if ((type == 4) && required_enable) {
        // int ret = iio_device_debug_attr_write(dev, "direct_reg_access", "enabled");
        int ret = iio_device_attr_write(dev, "reg_access", "enabled");

        if (ret < 0) {
            char error[1024];
            sprintf(error,
                    "Failed to enabled register for device: %s [%d]",
                    device.c_str(),
                    ret);
            throw std::runtime_error(error);
        }
    }

    message_port_register_in(pmt::mp("attr"));
    set_msg_handler(pmt::mp("attr"),
                    boost::bind(&attr_sink_impl::write_attribute, this, _1));
}

/*
 * Our virtual destructor.
 */
attr_sink_impl::~attr_sink_impl() {}

void attr_sink_impl::write_attribute(pmt::pmt_t pdu)
{
    int ret = 0;
    uint16_t k;
    std::string value, attribute;
    pmt::pmt_t keys;

    if (!is_dict(pdu))
        throw std::runtime_error("Message not a dictionary!\n");

    keys = pmt::dict_keys(pdu);

    for (k = 0; k < pmt::length(keys); k++) {

        attribute = pmt::symbol_to_string(pmt::nth(k, keys));
        value =
            pmt::symbol_to_string(pmt::dict_ref(pdu, pmt::nth(k, keys), pmt::PMT_NIL));

        switch (type) {
        case 0:
            ret = iio_channel_attr_write(chan, attribute.c_str(), value.c_str());
            break;
        case 1:
            ret = iio_device_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        case 2:
            ret = iio_device_buffer_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        case 3:
            ret = iio_device_debug_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        default: // case 4:
            try {
                uint32_t address = boost::lexical_cast<uint32_t>(attribute);
                uint32_t value32 = boost::lexical_cast<uint32_t>(value);
                ret = iio_device_reg_write(dev, address, value32);
            } catch (const boost::bad_lexical_cast& e) {
                std::cerr << e.what() << '\n';
            }
            break;
        }

        if (ret < 0) {
            char error[1024];
            sprintf(error,
                    "Attribute write '%s' failed to %s:%s:%s\n",
                    value.c_str(),
                    device.c_str(),
                    channel.c_str(),
                    attribute.c_str());
            throw std::runtime_error(error);
        }
    }
}


} /* namespace iio */
} /* namespace gr */
