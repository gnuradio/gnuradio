/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_DEVICE_SINK_H
#define INCLUDED_IIO_DEVICE_SINK_H

#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include <string>

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
    typedef std::shared_ptr<device_sink> sptr;

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

    /*!
     * The key of the tag that indicates packet length.
     * When not empty, samples are expected as "packets" and
     * must be tagged as such, i.e. the first sample of a packet needs to be
     * tagged with the corresponding length of that packet.
     * Note, packet size MUST be equal to buffer_size / (1+interpolation),
     * otherwise a runtime_error will be thrown. This is a deliberate design
     * choice, because all other options would result in potentially unexpected
     * behavior.
     */
    virtual void set_len_tag_key(const std::string& len_tag_key) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SINK_H */
