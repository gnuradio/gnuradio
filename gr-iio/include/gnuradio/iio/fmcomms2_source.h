/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
    typedef std::shared_ptr<fmcomms2_source> sptr;

    // /*!
    //  * \brief Return a shared_ptr to a new instance of iio::fmcomms2_source.
    //  *
    //  * \param uri  String of the context uri
    //  * \param frequency  Long long of LO frequency in Hz
    //  * \param samplerate  Long of sample rate in samples per second
    //  * \param bandwidth  Long of bandwidth of front-end analog filter  in
    //  *                   in Hz
    //  * \param ch1_en  Boolean enable channel 1
    //  * \param ch2_en  Boolean enable channel 2
    //  * \param ch3_en  Boolean enable channel 3
    //  * \param ch4_en  Boolean enable channel 4
    //  * \param quadrature  Boolean enable RX quadrature tracking
    //  * \param rfdc  Boolean enable RX RF DC tracking
    //  * \param bbdc  Boolean enable RX Baseband DC tracking
    //  * \param buffer_size  Long of number of samples in buffer to send to device
    //  * \param rf_port_select  String of name of port to use for TX output mux
    //  *        with options:
    //  *        - 'A_BALANCED'
    //  *        - 'B_BALANCED'
    //  *        - 'C_BALANCED'
    //  *        - 'A_N'
    //  *        - 'A_P'
    //  *        - 'B_N'
    //  *        - 'B_P'
    //  *        - 'C_N'
    //  *        - 'C_P'
    //  *        - 'TX_MONITOR1'
    //  *        - 'TX_MONITOR2'
    //  *        - 'TX_MONITOR1_2'
    //  * \param gain1  String of gain mode for channel 1 with options:
    //  *        - 'manual'
    //  *        - 'slow_attack'
    //  *        - 'fast_attack'
    //  *        - 'hybrid'
    //  * \param gain1_value  Double of RX channel 1 gain in dB [0, 76]
    //  * \param gain2  String of gain mode for channel 1 with options:
    //  *        - 'manual'
    //  *        - 'slow_attack'
    //  *        - 'fast_attack'
    //  *        - 'hybrid'
    //  * \param gain2_value  Double of RX channel 2 gain in dB [0, 76]
    //  * \param filter_source  String which selects filter configuration with
    //  *        options:
    //  *        - 'Off': Disable filter
    //  *        - 'Auto': Use auto-generated filters
    //  *        - 'File': Use provide filter filter in filter_filename input
    //  *        - 'Design': Create filter from Fpass, Fstop, samplerate, and
    //  *                  bandwidth parameters
    //  * \param filter_filename  String of path to filter file
    //  * \param Fpass Float of edge of passband frequency in Hz for designed FIR
    //  * \param Fstop Float of edge of stopband frequency in Hz for designed FIR
    //  */
    // static sptr make(const std::string& uri,
    //                  unsigned long long frequency,
    //                  unsigned long samplerate,
    //                  unsigned long bandwidth,
    //                  bool ch1_en,
    //                  bool ch2_en,
    //                  bool ch3_en,
    //                  bool ch4_en,
    //                  unsigned long buffer_size,
    //                  bool quadrature,
    //                  bool rfdc,
    //                  bool bbdc,
    //                  const char* gain1,
    //                  double gain1_value,
    //                  const char* gain2,
    //                  double gain2_value,
    //                  const char* rf_port_select,
    //                  const char* filter_source = "",
    //                  const char* filter_filename = "",
    //                  float Fpass = 0.0,
    //                  float Fstop = 0.0);
    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size);

    // virtual void set_params(unsigned long long frequency,
    //                         unsigned long samplerate,
    //                         unsigned long bandwidth,
    //                         bool quadrature,
    //                         bool rfdc,
    //                         bool bbdc,
    //                         const char* gain1,
    //                         double gain1_value,
    //                         const char* gain2,
    //                         double gain2_value,
    //                         const char* rf_port_select,
    //                         const char* filter_source = "",
    //                         const char* filter_filename = "",
    //                         float Fpass = 0.0,
    //                         float Fstop = 0.0) = 0;

    virtual void set_frequency(unsigned long long frequency) = 0;
    virtual void set_samplerate(unsigned long samplerate) = 0;
    virtual void set_gain_mode(size_t chan, const std::string& mode) = 0;
    virtual void set_gain(size_t chan, double gain) = 0;
    virtual void set_quadrature(bool quadrature) = 0;
    virtual void set_rfdc(bool rfdc) = 0;
    virtual void set_bbdc(bool bbdc) = 0;
    virtual void set_filter_source(const std::string& filter_source) = 0;
    virtual void set_filter_filename(const std::string& filter_filename) = 0;
    virtual void set_fpass(float fpass) = 0;
    virtual void set_fstop(float fstop) = 0;
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
    typedef std::shared_ptr<fmcomms2_source_f32c> sptr;

    static sptr make(const std::string& uri,
                     const std::vector<bool>& ch_en,
                     unsigned long buffer_size)
    {
        fmcomms2_source::sptr block = fmcomms2_source::make(uri, ch_en, buffer_size);

        return gnuradio::get_initial_sptr(new fmcomms2_source_f32c(
            (ch_en.size() > 0 && ch_en[0]), (ch_en.size() > 1 && ch_en[1]), block));
    }

    virtual void set_frequency(unsigned long long frequency)
    {
        fmcomms2_block->set_frequency(frequency);
    }
    virtual void set_samplerate(unsigned long samplerate)
    {
        fmcomms2_block->set_samplerate(samplerate);
    }
    virtual void set_gain_mode(size_t chan, const std::string& mode)
    {
        fmcomms2_block->set_gain_mode(chan, mode);
    }
    virtual void set_gain(size_t chan, double gain)
    {
        fmcomms2_block->set_gain(chan, gain);
    }
    virtual void set_quadrature(bool quadrature)
    {
        fmcomms2_block->set_quadrature(quadrature);
    }
    virtual void set_rfdc(bool rfdc) { fmcomms2_block->set_rfdc(rfdc); }
    virtual void set_bbdc(bool bbdc) { fmcomms2_block->set_bbdc(bbdc); }
    virtual void set_filter_source(const std::string& filter_source)
    {
        fmcomms2_block->set_filter_source(filter_source);
    }
    virtual void set_filter_filename(const std::string& filter_filename)
    {
        fmcomms2_block->set_filter_filename(filter_filename);
    }
    virtual void set_fpass(float fpass) { fmcomms2_block->set_fpass(fpass); }
    virtual void set_fstop(float fstop) { fmcomms2_block->set_fstop(fstop); }

private:
    fmcomms2_source::sptr fmcomms2_block;

protected:
    explicit fmcomms2_source_f32c(bool rx1_en, bool rx2_en, fmcomms2_source::sptr block);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_H */
