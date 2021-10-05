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
#include <gnuradio/iio/fmcomms2_sink.h>
namespace gr {
namespace iio {
/*!
 * \brief Sink block for the PlutoSDR
 * \ingroup iio
 *
 */
class IIO_API pluto_sink : virtual public fmcomms2_sink<gr_complex>
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

};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SINK_H */
