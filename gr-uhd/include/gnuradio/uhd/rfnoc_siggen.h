/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_SIGGEN_H
#define INCLUDED_GR_UHD_RFNOC_SIGGEN_H

#include <gnuradio/uhd/api.h>
#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {

/*! RFNoC Signal Generator Block
 *
 * \ingroup uhd_blk
 */
class GR_UHD_API rfnoc_siggen : virtual public rfnoc_block
{
public:
    typedef std::shared_ptr<rfnoc_siggen> sptr;

    enum class siggen_waveform { CONSTANT, SINE_WAVE, NOISE };

    /*!
     * \param graph Reference to the rfnoc_graph object this block is attached to
     * \param block_args Additional block arguments
     * \param device_select Device Selection
     * \param instance Instance Selection
     */
    static sptr make(rfnoc_graph::sptr graph,
                     const ::uhd::device_addr_t& block_args,
                     const int device_select,
                     const int instance);

    /*! Set the amplitude of the signal
     *
     * \param amplitude Amplitude of the signal (double)
     * \param chan      Channel index
     */
    virtual void set_amplitude(const double amplitude, const size_t chan) = 0;

    /*! Get the amplitude of the signal
     *
     * \param chan  Channel index
     * \returns     Signal amplitude
     */
    virtual double get_amplitude(const size_t chan) = 0;

    /*! Set a complex constant of the signal
     *
     * \param constant  Constant for the signal (complex double)
     * \param chan      Channel index
     */
    virtual void set_constant(const std::complex<double> constant, const size_t chan) = 0;

    /*! Get the complex constant of the signal
     *
     * \param chan  Channel index
     * \returns     Complex constant of signal
     */
    virtual std::complex<double> get_constant(const size_t chan) = 0;

    /*! Enable the channel
     *
     * \param enable    Boolean to enable or disable output
     * \param chan      Channel index
     */
    virtual void set_enable(const bool enable, const size_t chan) = 0;

    /*! Get the channel enable state
     *
     * \param chan  Channel index
     * \returns     Enable State
     */
    virtual bool get_enable(const size_t chan) = 0;

    /*! Set the sine frequency in terms of the sample_rate
     *
     * \param frequency     The frequency of the tone being set
     * \param sample_rate   The sample rate of the block
     * \param chan          Channel index
     */
    virtual void set_sine_frequency(const double frequency,
                                    const double sample_rate,
                                    const size_t chan) = 0;

    /*! Set the sine frequency phase increment
     *
     * \param phase_inc The normalized phase increment per sample
     * \param chan      Channel index
     */
    virtual void set_sine_phase_increment(const double phase_inc, const size_t chan) = 0;

    /*! Get the sine frequency phase increment
     *
     * \param chan  Channel index
     * \returns     The normalized phase increment per sample
     */
    virtual double get_sine_phase_increment(const size_t chan) = 0;

    /*! Set the type of waveform using the siggen_waveform class
     *
     * \param type The waveform to choose
     * \param chan Channel index
     */
    virtual void set_waveform(const siggen_waveform type, const size_t chan) = 0;

    /*! Set the type of waveform using a string
     *
     * \param type  The waveform to choose ["CONSTANT", "SINE_WAVE", "NOISE"]
     * \param chan  Channel index
     */
    virtual void set_waveform(const std::string& type, const size_t chan) = 0;

    /*! Get the type of waveform as a string
     *
     * \param chan  Channel index
     * \returns     The current waveform as a string
     */
    virtual std::string get_waveform_string(const size_t chan) = 0;

    /*! Get the waveform
     *
     * \param chan  Channel index
     * \returns     The current waveform
     */
    virtual siggen_waveform get_waveform(const size_t chan) = 0;

    /*! Set the number of samples per packet
     *
     * \param spp   The number of samples per packet
     * \param chan  Channel index
     */
    virtual void set_samples_per_packet(const size_t spp, const size_t chan) = 0;

    /*! Get the number of samples per packet
     *
     * \param chan  Channel index
     * \returns     The number of samples per packet
     */
    virtual size_t get_samples_per_packet(const size_t chan) = 0;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_SIGGEN_H */
