/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "attr_sink_impl.h"
#include <gnuradio/io_signature.h>

#include <sstream>
#include <string>

namespace gr {
namespace iio {

attr_sink::sptr attr_sink::make(const std::string& uri,
                                const std::string& device,
                                const std::string& channel,
                                attr_type_t type,
                                bool output)
{
    return gnuradio::make_block_sptr<attr_sink_impl>(uri, device, channel, type, output);
}

/*
 * The private constructor
 */
attr_sink_impl::attr_sink_impl(const std::string& uri,
                               const std::string& device,
                               const std::string& channel,
                               attr_type_t type,
                               bool output)
    : gr::block(
          "attr_sink", gr::io_signature::make(0, 0, 0), gr::io_signature::make(0, 0, 0)),
      device(device),
      channel(channel),
      uri(uri),
      type(type)
{

    ctx = device_source_impl::get_context(uri);
    if (!ctx)
        throw std::runtime_error("Unable to create context");

    dev = iio_context_find_device(ctx, device.c_str());
    if (!dev) {
        iio_context_destroy(ctx);
        throw std::runtime_error("Device not found");
    }

    if (type == attr_type_t::CHANNEL) {
        chan = iio_device_find_channel(dev, channel.c_str(), output);
        if (!chan) {
            iio_context_destroy(ctx);
            throw std::runtime_error("Channel not found");
        }
    }

    message_port_register_in(pmt::mp("attr"));
    set_msg_handler(pmt::mp("attr"),
                    [this](pmt::pmt_t pdu) { this->write_attribute(pdu); });
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
        case attr_type_t::CHANNEL:
            ret = iio_channel_attr_write(chan, attribute.c_str(), value.c_str());
            break;
        case attr_type_t::DEVICE:
            ret = iio_device_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        case attr_type_t::DEVICE_BUFFER:
            ret = iio_device_buffer_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        case attr_type_t::DEVICE_DEBUG:
            ret = iio_device_debug_attr_write(dev, attribute.c_str(), value.c_str());
            break;
        default: // case 4:
            uint32_t address, value32;

            std::istringstream attribute_is(attribute);
            attribute_is >> address;

            std::istringstream value_is(value);
            value_is >> value32;

            if (attribute_is.fail()) {
                d_logger->warn("Failed to convert {:s} to uint32_t", attribute);
            } else if (value_is.fail()) {
                d_logger->warn("Failed to convert {:s} to uint32_t", value);
            } else {
                ret = iio_device_reg_write(dev, address, value32);
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
