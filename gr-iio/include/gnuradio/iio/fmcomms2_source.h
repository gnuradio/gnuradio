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


#ifndef INCLUDED_IIO_FMCOMMS2_SOURCE_H
#define INCLUDED_IIO_FMCOMMS2_SOURCE_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/iio/api.h>
#include <gnuradio/sync_block.h>

#include "device_source.h"

namespace gr {
namespace iio {

/*!
 * \brief Device specific source for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_source : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<fmcomms2_source> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms2_source.
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
     * \param gain2  String of gain mode for channel 1 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain2_value  Double of RX channel 2 gain in dB [0, 76]
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
                     bool quadrature,
                     bool rfdc,
                     bool bbdc,
                     const char* gain1,
                     double gain1_value,
                     const char* gain2,
                     double gain2_value,
                     const char* rf_port_select,
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
                          bool quadrature,
                          bool rfdc,
                          bool bbdc,
                          const char* gain1,
                          double gain1_value,
                          const char* gain2,
                          double gain2_value,
                          const char* rf_port_select,
                          const char* filter_source = "",
                          const char* filter_filename = "",
                          float Fpass = 0.0,
                          float Fstop = 0.0);

    virtual void set_params(unsigned long long frequency,
                            unsigned long samplerate,
                            unsigned long bandwidth,
                            bool quadrature,
                            bool rfdc,
                            bool bbdc,
                            const char* gain1,
                            double gain1_value,
                            const char* gain2,
                            double gain2_value,
                            const char* rf_port_select,
                            const char* filter_source = "",
                            const char* filter_filename = "",
                            float Fpass = 0.0,
                            float Fstop = 0.0) = 0;
};

/*!
 * \brief Device specific source for FMComms evaluation cards
 * \ingroup iio
 *
 * \details
 * This block is a source specifically designed for FMComms2/3/4 evaluation
 * cards. However, it should support any AD936x based device using an IIO
 * driver.
 */
class IIO_API fmcomms2_source_f32c : virtual public gr::hier_block2
{
public:
    typedef boost::shared_ptr<fmcomms2_source_f32c> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of iio::fmcomms2_source.
     *
     * \param uri  String of the context uri
     * \param frequency  Long long of LO frequency in Hz
     * \param samplerate  Long of sample rate in samples per second
     * \param bandwidth  Long of bandwidth of front-end analog filter  in
     *                   in Hz
     * \param rx1_en  Boolean enable channel 1
     * \param rx2_en  Boolean enable channel 2
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
     * \param gain2  String of gain mode for channel 1 with options:
     *        - 'manual'
     *        - 'slow_attack'
     *        - 'fast_attack'
     *        - 'hybrid'
     * \param gain2_value  Double of RX channel 2 gain in dB [0, 76]
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
                     bool rx1_en,
                     bool rx2_en,
                     unsigned long buffer_size,
                     bool quadrature,
                     bool rfdc,
                     bool bbdc,
                     const char* gain1,
                     double gain1_value,
                     const char* gain2,
                     double gain2_value,
                     const char* rf_port_select,
                     const char* filter_source = "",
                     const char* filter_filename = "",
                     float Fpass = 0.0,
                     float Fstop = 0.0)
    {
        fmcomms2_source::sptr block = fmcomms2_source::make(uri,
                                                            frequency,
                                                            samplerate,
                                                            bandwidth,
                                                            rx1_en,
                                                            rx1_en,
                                                            rx2_en,
                                                            rx2_en,
                                                            buffer_size,
                                                            quadrature,
                                                            rfdc,
                                                            bbdc,
                                                            gain1,
                                                            gain1_value,
                                                            gain2,
                                                            gain2_value,
                                                            rf_port_select,
                                                            filter_source,
                                                            filter_filename,
                                                            Fpass,
                                                            Fstop);

        return gnuradio::get_initial_sptr(
            new fmcomms2_source_f32c(rx1_en, rx2_en, block));
    }

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    bool quadrature,
                    bool rfdc,
                    bool bbdc,
                    const char* gain1,
                    double gain1_value,
                    const char* gain2,
                    double gain2_value,
                    const char* rf_port_select,
                    const char* filter_source = "",
                    const char* filter_filename = "",
                    float Fpass = 0.0,
                    float Fstop = 0.0)
    {
        fmcomms2_block->set_params(frequency,
                                   samplerate,
                                   bandwidth,
                                   quadrature,
                                   rfdc,
                                   bbdc,
                                   gain1,
                                   gain1_value,
                                   gain2,
                                   gain2_value,
                                   rf_port_select,
                                   filter_source,
                                   filter_filename,
                                   Fpass,
                                   Fstop);
    }

private:
    fmcomms2_source::sptr fmcomms2_block;

protected:
    explicit fmcomms2_source_f32c(bool rx1_en, bool rx2_en, fmcomms2_source::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_H */
