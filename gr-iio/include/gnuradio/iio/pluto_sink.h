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
     * \param frequency  Long long of LO frequency in Hz
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     * \param attenuation  Double of TX channel attenuation in dB [0, 90]
     * \param filter_source  String which selects filter configuration with
     *        options:
     *        - 'Off': Disable filter
     *        - 'Auto': Use auto-generated filters
     *        - 'File': Use provide filter filter in filter_filename input
     *        - 'Design': Create filter from Fpass, Fstop, samplerate, and
     *                  bandwidth parameters
     * \param filter_filename  String of path to filter file
     * \param Fpass Float of edge of passband frequency in Hz for designed FIR
     * \param Fstop Float of edge of stopband frequency in Hz for designed FIR
     */
    static sptr make(const std::string& uri,
                     unsigned long long frequency,
                     unsigned long samplerate,
                     unsigned long bandwidth,
                     unsigned long buffer_size,
                     bool cyclic,
                     double attenuation,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0);

    virtual void set_params(unsigned long long frequency,
                            unsigned long samplerate,
                            unsigned long bandwidth,
                            double attenuation,
                            const char* filter_source = "",
                            const char* filter_filename = "",
                            float Fpass = 0.0,
                            float Fstop = 0.0) = 0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SINK_H */
