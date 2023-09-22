/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_ATTR_SINK_H
#define INCLUDED_IIO_ATTR_SINK_H

#include <gnuradio/block.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/iio/iio_types.h>

namespace gr {
namespace iio {

/*!
 * \brief Generic writer for attributes of IIO devices
 * \ingroup iio
 *
 * \details
 * This block allow for updating of any IIO attribute that is writable. This
 * includes channel, device, device buffer, device debug, and direct register
 * attributes. All messages must be a pmt dictionary where the key is the
 * attribute to update and the value is the value to be written. Messages can
 * be an array of dictionaries or a single dictionary.
 */
class IIO_API attr_sink : virtual public gr::block
{
public:
    typedef std::shared_ptr<attr_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::attr_sink.
     *
     * \param uri String of context uri
     * \param device String of device name
     * \param channel String of device name
     * \param type Integer determining attribute type:
     *        0: Channel attribute
     *        1: Device attribute
     *        2: Device buffer attribute
     *        3: Device debug attribute
     *        4: Direct register access
     * \param output Boolean when True if channel attribute is an output
     */
    static sptr make(const std::string& uri,
                     const std::string& device,
                     const std::string& channel,
                     attr_type_t type,
                     bool output);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SINK_H */
