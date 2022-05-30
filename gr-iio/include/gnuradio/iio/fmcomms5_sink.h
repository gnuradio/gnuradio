/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS5_SINK_H
#define INCLUDED_IIO_FMCOMMS5_SINK_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_sink.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific sink for FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */
class IIO_API fmcomms5_sink : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<fmcomms5_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms5_sink.
     *
     * \param uri  String of the context uri
     * \param frequency1  Long long of LO frequency in Hz of chip A
     * \param frequency2  Long long of LO frequency in Hz of chip B
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param ch1_en  Boolean enable channel 1
     * \param ch2_en  Boolean enable channel 2
     * \param ch3_en  Boolean enable channel 3
     * \param ch4_en  Boolean enable channel 4
     * \param ch5_en  Boolean enable channel 5
     * \param ch6_en  Boolean enable channel 6
     * \param ch7_en  Boolean enable channel 7
     * \param ch8_en  Boolean enable channel 8
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options: 'A', 'B'
     * \param attenuation1  Double of TX channel 1 attenuation in dB [0, 90]
     * \param attenuation2  Double of TX channel 2 attenuation in dB [0, 90]
     * \param attenuation3  Double of TX channel 3 attenuation in dB [0, 90]
     * \param attenuation4  Double of TX channel 4 attenuation in dB [0, 90]
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
                     unsigned long long frequency1,
                     unsigned long long frequency2,
                     unsigned long samplerate,
                     unsigned long bandwidth,
                     bool ch1_en,
                     bool ch2_en,
                     bool ch3_en,
                     bool ch4_en,
                     bool ch5_en,
                     bool ch6_en,
                     bool ch7_en,
                     bool ch8_en,
                     unsigned long buffer_size,
                     bool cyclic,
                     const char* rf_port_select,
                     double attenuation1,
                     double attenuation2,
                     double attenuation3,
                     double attenuation4,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0);

    static sptr make_from(iio_context* ctx,
                          unsigned long long frequency1,
                          unsigned long long frequency2,
                          unsigned long samplerate,
                          unsigned long bandwidth,
                          bool ch1_en,
                          bool ch2_en,
                          bool ch3_en,
                          bool ch4_en,
                          bool ch5_en,
                          bool ch6_en,
                          bool ch7_en,
                          bool ch8_en,
                          unsigned long buffer_size,
                          bool cyclic,
                          const char* rf_port_select,
                          double attenuation1,
                          double attenuation2,
                          double attenuation3,
                          double attenuation4,
                          const char* filter_source = "",
                          const char* filter_filename = "",
                          float Fpass = 0.0,
                          float Fstop = 0.0);

    virtual void set_params(unsigned long long frequency1,
                            unsigned long long frequency2,
                            unsigned long samplerate,
                            unsigned long bandwidth,
                            const char* rf_port_select,
                            double attenuation1,
                            double attenuation2,
                            double attenuation3,
                            double attenuation4,
                            const char* filter_source,
                            const char* filter_filename,
                            float Fpass,
                            float Fstop) = 0;
};

/*!
 * \brief Device specific sink for FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a sink specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */
class IIO_API fmcomms5_sink_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<fmcomms5_sink_f32c> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms5_sink.
     *
     * \param uri  String of the context uri
     * \param frequency1  Long long of LO frequency in Hz of chip A
     * \param frequency2  Long long of LO frequency in Hz of chip B
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param tx1_en  Boolean enable channel 1
     * \param tx2_en  Boolean enable channel 2
     * \param tx3_en  Boolean enable channel 3
     * \param tx4_en  Boolean enable channel 4
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param cyclic Boolean when True sends first buffer_size number of samples
     *        to hardware which is repeated in the hardware itself. Future
     *        samples are ignored.
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options: 'A', 'B'
     * \param attenuation1  Double of TX channel 1 attenuation in dB [0, 90]
     * \param attenuation2  Double of TX channel 2 attenuation in dB [0, 90]
     * \param attenuation3  Double of TX channel 3 attenuation in dB [0, 90]
     * \param attenuation4  Double of TX channel 4 attenuation in dB [0, 90]
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
                     unsigned long long frequency1,
                     unsigned long long frequency2,
                     unsigned long samplerate,
                     unsigned long bandwidth,
                     bool tx1_en,
                     bool tx2_en,
                     bool tx3_en,
                     bool tx4_en,
                     unsigned long buffer_size,
                     bool cyclic,
                     const char* rf_port_select,
                     double attenuation1,
                     double attenuation2,
                     double attenuation3,
                     double attenuation4,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0)
    {
        fmcomms5_sink::sptr block = fmcomms5_sink::make(uri,
                                                        frequency1,
                                                        frequency2,
                                                        samplerate,
                                                        bandwidth,
                                                        tx1_en,
                                                        tx1_en,
                                                        tx2_en,
                                                        tx2_en,
                                                        tx3_en,
                                                        tx3_en,
                                                        tx4_en,
                                                        tx4_en,
                                                        buffer_size,
                                                        cyclic,
                                                        rf_port_select,
                                                        attenuation1,
                                                        attenuation2,
                                                        attenuation3,
                                                        attenuation4,
                                                        filter_source,
                                                        filter_filename,
                                                        Fpass,
                                                        Fstop);

        return gnuradio::make_block_sptr<fmcomms5_sink_f32c>(
            tx1_en, tx2_en, tx3_en, tx4_en, block);
    }

    void set_params(unsigned long long frequency1,
                    unsigned long long frequency2,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    const char* rf_port_select,
                    double attenuation1,
                    double attenuation2,
                    double attenuation3,
                    double attenuation4,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop)
    {
        fmcomms5_block->set_params(frequency1,
                                   frequency2,
                                   samplerate,
                                   bandwidth,
                                   rf_port_select,
                                   attenuation1,
                                   attenuation2,
                                   attenuation3,
                                   attenuation4,
                                   filter_source,
                                   filter_filename,
                                   Fpass,
                                   Fstop);
    }

private:
    fmcomms5_sink::sptr fmcomms5_block;

protected:
    explicit fmcomms5_sink_f32c(
        bool tx1_en, bool tx2_en, bool tx3_en, bool tx4_en, fmcomms5_sink::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SINK_H */
