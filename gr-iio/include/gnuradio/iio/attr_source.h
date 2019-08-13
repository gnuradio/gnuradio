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


#ifndef INCLUDED_IIO_ATTR_SOURCE_H
#define INCLUDED_IIO_ATTR_SOURCE_H

#include <gnuradio/iio/api.h>
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
    typedef boost::shared_ptr<attr_source> sptr;

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
     * \param data_type  Integer which selects what data type to output for
     *        received data:
     *        0: double
     *        1: float
     *        2: long long
     *        3: int
     *        4: uint8
     * \param attr_type  Integer determining attribute type:
     *        0: Channel attribute
     *        1: Device attribute
     *        2: Device debug attribute
     * \param output  Boolean when True if channel attribute is an output
     * \param address  uint32 register address of register to be read
     * \param required_enable  Boolean when True if an extra register_access
     *        attribute write is required for use a register. This is required
     *        for MathWorks generated IP.
     */
    static sptr make(const std::string& uri,
                     const std::string& device,
                     const std::string& channel,
                     const std::string& attribute,
                     int update_interval_ms,
                     int samples_per_update,
                     int data_type,
                     int attr_type,
                     bool output,
                     uint32_t address,
                     bool required_enable);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SOURCE_H */
