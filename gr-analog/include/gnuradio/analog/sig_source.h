/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SIG_SOURCE_H
#define SIG_SOURCE_H

#include <gnuradio/analog/api.h>
#include <gnuradio/analog/sig_source_waveform.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

/*!
 * \brief signal generator with T output.
 * \ingroup waveform_generators_blk
 */
template <class T>
class ANALOG_API sig_source : virtual public sync_block
{
public:
    // gr::analog::sig_source::sptr
    typedef std::shared_ptr<sig_source<T>> sptr;

    /*!
     * Build a signal source block.
     *
     * \param sampling_freq Sampling rate of signal.
     * \param waveform wavetform type.
     * \param wave_freq Frequency of waveform (relative to sampling_freq).
     * \param ampl Signal amplitude.
     * \param offset offset of signal.
     * \param phase Initial phase of the signal
     */
    static sptr make(double sampling_freq,
                     gr::analog::gr_waveform_t waveform,
                     double wave_freq,
                     double ampl,
                     T offset = 0,
                     float phase = 0);

    virtual double sampling_freq() const = 0;
    virtual gr::analog::gr_waveform_t waveform() const = 0;
    virtual double frequency() const = 0;
    virtual double amplitude() const = 0;
    virtual T offset() const = 0;
    virtual float phase() const = 0;

    /*!
     * Sets the sampling frequency of the signal.
     * \param sampling_freq sampling frequency
     */
    virtual void set_sampling_freq(double sampling_freq) = 0;

    /*!
     * Sets the waveform type of signal.
     * \param waveform waveform type
     */
    virtual void set_waveform(gr::analog::gr_waveform_t waveform) = 0;

    /*!
     * Sets the frequency of a periodic signal.
     * \param frequency frequency of the signal
     */
    virtual void set_frequency(double frequency) = 0;

    /*!
     * Sets the amplitude of a signal.
     * \param ampl amplitude of the signal
     */
    virtual void set_amplitude(double ampl) = 0;

    /*!
     * Sets the DC offset of a signal.
     * \param offset DC offset of the signal
     */
    virtual void set_offset(T offset) = 0;

    /*!
     * Sets the phase of a periodic signal.
     * \param phase phase of the signal
     */
    virtual void set_phase(float phase) = 0;
};

typedef sig_source<std::int8_t> sig_source_b;
typedef sig_source<std::int16_t> sig_source_s;
typedef sig_source<std::int32_t> sig_source_i;
typedef sig_source<float> sig_source_f;
typedef sig_source<gr_complex> sig_source_c;
} /* namespace analog */
} /* namespace gr */

#endif /* SIG_SOURCE_H */
