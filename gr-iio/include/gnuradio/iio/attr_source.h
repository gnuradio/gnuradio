/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_ATTR_SOURCE_H
#define INCLUDED_IIO_ATTR_SOURCE_H

#include <gnuradio/iio/api.h>
#include <gnuradio/iio/iio_types.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace iio {

/*!
 * \brief Generic reader for attributes of IIO devices
 * \ingroup iio
 *
 * \details
 * This block allow for reading of any IIO attribute that is readable. This
 * includes channel, device, device buffer, device debug, and direct register
 * attributes.
 */
class IIO_API attr_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<attr_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::attr_source.
     *
     * \param uri  String of the context uri
     * \param device  String of device name
     * \param channel  String of device name
     * \param attribute  String of attribute name
     * \param update_interval_ms  Integer number of milliseconds between
     *        attribute reads
     * \param samples_per_update  Integer number of samples to collect before
     *        block returns
     * \param data_type  Enum which selects what data type to output for
     *        received data:
     *        0: double
     *        1: float
     *        2: long long
     *        3: int
     *        4: uint8
     * \param attr_type  Enum determining attribute type:
     *        0: Channel attribute
     *        1: Device attribute
     *        2: Device debug attribute
     * \param output  Boolean when True if channel attribute is an output
     * \param address  uint32 register address of register to be read
     */
    static sptr make(const std::string& uri,
                     const std::string& device,
                     const std::string& channel,
                     const std::string& attribute,
                     int update_interval_ms,
                     int samples_per_update,
                     data_type_t data_type,
                     attr_type_t attr_type,
                     bool output,
                     uint32_t address);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SOURCE_H */
