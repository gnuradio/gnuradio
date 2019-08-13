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


#ifndef INCLUDED_IIO_FMCOMMS2_SINK_H
#define INCLUDED_IIO_FMCOMMS2_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_sink.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific sink for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_sink : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<fmcomms2_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms2_sink.
     *
     * \param uri  String of the context uri
     * \param frequency  Long long of LO frequency in Hz
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param ch1_en  Boolean enable channel 1
     * \param ch2_en  Boolean enable channel 2
     * \param ch3_en  Boolean enable channel 3
     * \param ch4_en  Boolean enable channel 4
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options: 'A', 'B'
     * \param attenuation1  Double of TX channel 1 attenuation in dB [0, 90]
     * \param attenuation2  Double of TX channel 2 attenuation in dB [0, 90]
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
                     bool ch1_en,
                     bool ch2_en,
                     bool ch3_en,
                     bool ch4_en,
                     unsigned long buffer_size,
                     bool cyclic,
                     const char* rf_port_select,
                     double attenuation1,
                     double attenuation2,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0);

    static sptr make_from(struct iio_context* ctx,
                          unsigned long long frequency,
                          unsigned long samplerate,
                          unsigned long bandwidth,
                          bool ch1_en,
                          bool ch2_en,
                          bool ch3_en,
                          bool ch4_en,
                          unsigned long buffer_size,
                          bool cyclic,
                          const char* rf_port_select,
                          double attenuation1,
                          double attenuation2,
                          const char* filter_source = "",
                          const char* filter_filename = "",
                          float Fpass = 0.0,
                          float Fstop = 0.0);

    virtual void set_params(unsigned long long frequency,
                            unsigned long samplerate,
                            unsigned long bandwidth,
                            const char* rf_port_select,
                            double attenuation1,
                            double attenuation2,
                            const char* filter_source = "",
                            const char* filter_filename = "",
                            float Fpass = 0.0,
                            float Fstop = 0.0) = 0;
};

/*!
 * \brief Device specific sink for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_sink_f32c : virtual public gr::hier_block2
{
public:
    typedef boost::shared_ptr<fmcomms2_sink_f32c> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms2_sink.
     *
     * \param uri  String of the context uri
     * \param frequency  Long long of LO frequency in Hz
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param tx1_en  Boolean enable channel 1
     * \param tx2_en  Boolean enable channel 2
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options: 'A', 'B'
     * \param attenuation1  Double of TX channel 1 attenuation in dB [0, 90]
     * \param attenuation2  Double of TX channel 2 attenuation in dB [0, 90]
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
                     bool tx1_en,
                     bool tx2_en,
                     unsigned long buffer_size,
                     bool cyclic,
                     const char* rf_port_select,
                     double attenuation1,
                     double attenuation2,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0)
    {
        fmcomms2_sink::sptr block = fmcomms2_sink::make(uri,
                                                        frequency,
                                                        samplerate,
                                                        bandwidth,
                                                        tx1_en,
                                                        tx1_en,
                                                        tx2_en,
                                                        tx2_en,
                                                        buffer_size,
                                                        cyclic,
                                                        rf_port_select,
                                                        attenuation1,
                                                        attenuation2,
                                                        filter_source,
                                                        filter_filename,
                                                        Fpass,
                                                        Fstop);

        return gnuradio::get_initial_sptr(new fmcomms2_sink_f32c(tx1_en, tx2_en, block));
    }

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    const char* rf_port_select,
                    double attenuation1,
                    double attenuation2,
                    const char* filter_source = "",
                    const char* filter_filename = "",
                    float Fpass = 0.0,
                    float Fstop = 0.0)
    {
        fmcomms2_block->set_params(frequency,
                                   samplerate,
                                   bandwidth,
                                   rf_port_select,
                                   attenuation1,
                                   attenuation2,
                                   filter_source,
                                   filter_filename,
                                   Fpass,
                                   Fstop);
    }

private:
    fmcomms2_sink::sptr fmcomms2_block;

protected:
    explicit fmcomms2_sink_f32c(bool tx1_en, bool tx2_en, fmcomms2_sink::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_H */
