/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
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


#ifndef INCLUDED_IIO_DEVICE_SINK_H
#define INCLUDED_IIO_DEVICE_SINK_H

#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#define DEFAULT_BUFFER_SIZE 0x8000

extern "C" {
struct iio_context;
};

namespace gr {
namespace iio {

/*!
 * \brief Generic sink for IIO drivers with buffered input channels
 * \ingroup iio
 *
 * \details
 * This block allows for streaming data to any IIO driver which has input
 * scan elements or buffered channels.
 */
class IIO_API device_sink : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<device_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::device.
     *
     * \param uri  String of the context uri
     * \param device  String of device name
     * \param channels  Vector of strings of channels names
     * \param device_phy String of phy device name where attribute updates are
     *        applied
     * \param params  Vector of strings of attributes to set in form:
     *        "<attribute name>=<value to set>,<attribute name>=<value to set>"
     * \param buffer_size Integer number of samples to be put into each IIO
     *        buffered passed to hardware.
     * \param interpolation Integer number of zeros to insert into transmit
     *        transmit buffers between samples
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     */
    static sptr make(const std::string& uri,
                     const std::string& device,
                     const std::vector<std::string>& channels,
                     const std::string& device_phy,
                     const std::vector<std::string>& params,
                     unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                     unsigned int interpolation = 0,
                     bool cyclic = false);

    static sptr make_from(struct iio_context* ctx,
                          const std::string& device,
                          const std::vector<std::string>& channels,
                          const std::string& device_phy,
                          const std::vector<std::string>& params,
                          unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                          unsigned int interpolation = 0,
                          bool cyclic = false);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SINK_H */
