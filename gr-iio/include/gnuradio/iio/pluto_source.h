/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_PLUTO_SOURCE_H
#define INCLUDED_IIO_PLUTO_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>

namespace gr {
namespace iio {
/*!
 * \brief Source block for the PlutoSDR
 * \ingroup iio
 *
 */
class IIO_API pluto_source : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<pluto_source> sptr;

    // /*!
    //  * \brief Return a shared_ptr to a new instance of iio::fmcomms2_source.
    //  *
    //  * \param uri  String of the context uri
    //  * \param frequency  Long long of LO frequency in Hz
    //  * \param samplerate  Long of sample rate in samples per second
    //  * \param bandwidth  Long of bandwidth of front-end analog filter  in
    //  *                   in Hz
    //  * \param quadrature  Boolean enable RX quadrature tracking
    //  * \param rfdc  Boolean enable RX RF DC tracking
    //  * \param bbdc  Boolean enable RX Baseband DC tracking
    //  * \param buffer_size  Long of number of samples in buffer to send to device
    //  * \param gain  String of gain mode with options:
    //  *        - 'manual'
    //  *        - 'slow_attack'
    //  *        - 'fast_attack'
    //  *        - 'hybrid'
    //  * \param gain_value  Double of RX channel in dB [0, 76]
    //  * \param filter_source  String which selects filter configuration with
    //  *        options:
    //  *        - 'Off': Disable filter
    //  *        - 'Auto': Use auto-generated filters
    //  *        - 'File': Use provide filter filter in filter_filename input
    //  *        - 'Design': Create filter from Fpass, Fstop, samplerate, and
    //  *        bandwidth parameters
    //  * \param filter_filename  String of path to filter file
    //  * \param Fpass Float of edge of passband frequency in Hz for designed FIR
    //  * \param Fstop Float of edge of stopband frequency in Hz for designed FIR
    //  */
    // static sptr make(const std::string& uri,
    //                  unsigned long long frequency,
    //                  unsigned long samplerate,
    //                  unsigned long bandwidth,
    //                  unsigned long buffer_size,
    //                  bool quadrature,
    //                  bool rfdc,
    //                  bool bbdc,
    //                  const char* gain,
    //                  double gain_value,
    //                  const char* filter_source = "",
    //                  const char* filter_filename = "",
    //                  float Fpass = 0.0,
    //                  float Fstop = 0.0);
    static sptr make(const std::string& uri, unsigned long buffer_size);

    // virtual void update_sample_rate(unsigned long samplerate) = 0;

    // virtual void set_params(unsigned long long frequency,
    //                         unsigned long samplerate,
    //                         unsigned long bandwidth,
    //                         bool quadrature,
    //                         bool rfdc,
    //                         bool bbdc,
    //                         const char* gain,
    //                         double gain_value,
    //                         const char* filter_source = "",
    //                         const char* filter_filename = "",
    //                         float Fpass = 0.0,
    //                         float Fstop = 0.0) = 0;

    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_gain_mode(const std::string& mode) = 0;
    virtual void set_gain(double gain) = 0;
    virtual void set_quadrature(bool quadrature) = 0;
    virtual void set_rfdc(bool rfdc) = 0;
    virtual void set_bbdc(bool bbdc) = 0;
    virtual void set_filter_source(const std::string& filter_source) = 0;
    virtual void set_filter_filename(const std::string& filter_filename) = 0;
    virtual void set_fpass(float fpass) = 0;
    virtual void set_fstop(float fstop) = 0;
};
} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_H */
