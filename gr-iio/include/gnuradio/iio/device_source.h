/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_DEVICE_SOURCE_H
#define INCLUDED_IIO_DEVICE_SOURCE_H

#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#define DEFAULT_BUFFER_SIZE 0x8000

extern "C" {
struct iio_context;
};

namespace gr {
namespace iio {

/*!
 * \brief Generic source for IIO drivers with buffered output channels
 * \ingroup iio
 *
 * \details
 * This block allows for streaming data from any IIO driver which has output
 * scan elements or buffered channels.
 */
class IIO_API device_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<device_source> sptr;

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
     * \param decimation Integer number of sample to remove from received
     *        data buffers between successive samples
     */
    static sptr make(const std::string& uri,
                     const std::string& device,
                     const std::vector<std::string>& channels,
                     const std::string& device_phy,
                     const std::vector<std::string>& params,
                     unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                     unsigned int decimation = 0);

    static sptr make_from(struct iio_context* ctx,
                          const std::string& device,
                          const std::vector<std::string>& channels,
                          const std::string& device_phy,
                          const std::vector<std::string>& params,
                          unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                          unsigned int decimation = 0);

    /*!
     * \brief Key of the packet length tag. If empty no tag will be emitted
     */
    virtual void set_len_tag_key(const std::string& len_tag_key) = 0;

    /*!
     * \brief Number of samples to be put into each IIO
     *        buffered passed to hardware.
     */
    virtual void set_buffer_size(unsigned int buffer_size) = 0;

    virtual void set_timeout_ms(unsigned long timeout) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SOURCE_H */
