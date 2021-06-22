/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_PLUTO_SINK_H
#define INCLUDED_IIO_PLUTO_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {
/*!
 * \brief Sink block for the PlutoSDR
 * \ingroup iio
 *
 */
class IIO_API pluto_sink : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<pluto_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms2_sink.
     *
     * \param uri  String of the context uri
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean, the first packet is repeated indefinitely when set
     */
    static sptr make(const std::string& uri, unsigned long buffer_size, bool cyclic);

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
    virtual void set_len_tag_key(const std::string& val) = 0;
    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_bandwidth(unsigned long bandwidth) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_attenuation(double attenuation) = 0;
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SINK_H */
