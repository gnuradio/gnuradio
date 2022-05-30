/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_IIO_FMCOMMS5_SOURCE_H
#define INCLUDED_IIO_FMCOMMS5_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_source.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific source for the FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */
class IIO_API fmcomms5_source : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<fmcomms5_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::device.
     *
     * \param uri  String of the context uri
     * \param frequency1  Long long of LO frequency in Hz of chip A
     * \param frequency2  Long long of LO frequency in Hz of chip A
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
     * \param quadrature  Boolean enable RX quadrature tracking
     * \param rfdc  Boolean enable RX RF DC tracking
     * \param bbdc  Boolean enable RX Baseband DC tracking
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options:
     *        - 'A_BALANCED'
     *        - 'B_BALANCED'
     *        - 'C_BALANCED'
     *        - 'A_N'
     *        - 'A_P'
     *        - 'B_N'
     *        - 'B_P'
     *        - 'C_N'
     *        - 'C_P'
     *        - 'TX_MONITOR1'
     *        - 'TX_MONITOR2'
     *        - 'TX_MONITOR1_2'
     * \param gain1  String of gain mode for channel 1 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain1_value  Double of RX channel 1 gain in dB [0, 76]
     * \param gain2  String of gain mode for channel 2 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain2_value  Double of RX channel 2 gain in dB [0, 76]
     * \param gain3  String of gain mode for channel 3 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain3_value  Double of RX channel 3 gain in dB [0, 76]
     * \param gain4  String of gain mode for channel 4 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain4_value  Double of RX channel 4 gain in dB [0, 76]
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
                     bool quadrature,
                     bool rfdc,
                     bool bbdc,
                     const char* gain1,
                     double gain1_value,
                     const char* gain2,
                     double gain2_value,
                     const char* gain3,
                     double gain3_value,
                     const char* gain4,
                     double gain4_value,
                     const char* rf_port_select,
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
                          bool quadrature,
                          bool rfdc,
                          bool bbdc,
                          const char* gain1,
                          double gain1_value,
                          const char* gain2,
                          double gain2_value,
                          const char* gain3,
                          double gain3_value,
                          const char* gain4,
                          double gain4_value,
                          const char* rf_port_select,
                          const char* filter_source = "",
                          const char* filter_filename = "",
                          float Fpass = 0.0,
                          float Fstop = 0.0);

    virtual void set_params(unsigned long long frequency1,
                            unsigned long long frequency2,
                            unsigned long samplerate,
                            unsigned long bandwidth,
                            bool quadrature,
                            bool rfdc,
                            bool bbdc,
                            const char* gain1,
                            double gain1_value,
                            const char* gain2,
                            double gain2_value,
                            const char* gain3,
                            double gain3_value,
                            const char* gain4,
                            double gain4_value,
                            const char* rf_port_select,
                            const char* filter_source,
                            const char* filter_filename,
                            float Fpass,
                            float Fstop) = 0;
};

/*!
 * \brief Device specific source for the FMComms5 evaluation card
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms5 evaluation
 * card. The FMComms5 is a dual AD9361 FMC card which enables 4x4
 * applications.
 */
class IIO_API fmcomms5_source_f32c : virtual public gr::hier_block2
{
public:
    typedef std::shared_ptr<fmcomms5_source_f32c> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::device.
     *
     * \param uri  String of the context uri
     * \param frequency1  Long long of LO frequency in Hz of chip A
     * \param frequency2  Long long of LO frequency in Hz of chip A
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param rx1_en  Boolean enable channel 1
     * \param rx2_en  Boolean enable channel 2
     * \param rx3_en  Boolean enable channel 3
     * \param rx4_en  Boolean enable channel 4
     * \param quadrature  Boolean enable RX quadrature tracking
     * \param rfdc  Boolean enable RX RF DC tracking
     * \param bbdc  Boolean enable RX Baseband DC tracking
     * \param buffer_size  Long of number of samples in buffer to send to device
     * \param rf_port_select  String of name of port to use for TX output mux
     *        with options:
     *        - 'A_BALANCED'
     *        - 'B_BALANCED'
     *        - 'C_BALANCED'
     *        - 'A_N'
     *        - 'A_P'
     *        - 'B_N'
     *        - 'B_P'
     *        - 'C_N'
     *        - 'C_P'
     *        - 'TX_MONITOR1'
     *        - 'TX_MONITOR2'
     *        - 'TX_MONITOR1_2'
     * \param gain1  String of gain mode for channel 1 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain1_value  Double of RX channel 1 gain in dB [0, 76]
     * \param gain2  String of gain mode for channel 2 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain2_value  Double of RX channel 2 gain in dB [0, 76]
     * \param gain3  String of gain mode for channel 3 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain3_value  Double of RX channel 3 gain in dB [0, 76]
     * \param gain4  String of gain mode for channel 4 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain4_value  Double of RX channel 4 gain in dB [0, 76]
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
                     bool rx1_en,
                     bool rx2_en,
                     bool rx3_en,
                     bool rx4_en,
                     unsigned long buffer_size,
                     bool quadrature,
                     bool rfdc,
                     bool bbdc,
                     const char* gain1,
                     double gain1_value,
                     const char* gain2,
                     double gain2_value,
                     const char* gain3,
                     double gain3_value,
                     const char* gain4,
                     double gain4_value,
                     const char* rf_port_select,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0)
    {
        fmcomms5_source::sptr block = fmcomms5_source::make(uri,
                                                            frequency1,
                                                            frequency2,
                                                            samplerate,
                                                            bandwidth,
                                                            rx1_en,
                                                            rx1_en,
                                                            rx2_en,
                                                            rx2_en,
                                                            rx3_en,
                                                            rx3_en,
                                                            rx4_en,
                                                            rx4_en,
                                                            buffer_size,
                                                            quadrature,
                                                            rfdc,
                                                            bbdc,
                                                            gain1,
                                                            gain1_value,
                                                            gain2,
                                                            gain2_value,
                                                            gain3,
                                                            gain3_value,
                                                            gain4,
                                                            gain4_value,
                                                            rf_port_select,
                                                            filter_source,
                                                            filter_filename,
                                                            Fpass,
                                                            Fstop);

        return gnuradio::make_block_sptr<fmcomms5_source_f32c>(
            rx1_en, rx2_en, rx3_en, rx4_en, block);
    }

    void set_params(unsigned long long frequency1,
                    unsigned long long frequency2,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    bool quadrature,
                    bool rfdc,
                    bool bbdc,
                    const char* gain1,
                    double gain1_value,
                    const char* gain2,
                    double gain2_value,
                    const char* gain3,
                    double gain3_value,
                    const char* gain4,
                    double gain4_value,
                    const char* rf_port_select,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop)
    {
        fmcomms5_block->set_params(frequency1,
                                   frequency2,
                                   samplerate,
                                   bandwidth,
                                   quadrature,
                                   rfdc,
                                   bbdc,
                                   gain1,
                                   gain1_value,
                                   gain2,
                                   gain2_value,
                                   gain3,
                                   gain3_value,
                                   gain4,
                                   gain4_value,
                                   rf_port_select,
                                   filter_source,
                                   filter_filename,
                                   Fpass,
                                   Fstop);
    }

private:
    fmcomms5_source::sptr fmcomms5_block;

protected:
    explicit fmcomms5_source_f32c(
        bool rx1_en, bool rx2_en, bool rx3_en, bool rx4_en, fmcomms5_source::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SOURCE_H */
