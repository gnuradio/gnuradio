/* -*- c++ -*- */
/*
 * Copyright 2006,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H
#define INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H

#include <gnuradio/digital/costas_loop_cc.h>

namespace gr {
namespace digital {

class costas_loop_cc_impl : public costas_loop_cc
{
private:
    float d_error;
    float d_noise;

    /*! \brief the phase detector circuit for 8th-order PSK loops.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_8(gr_complex sample) const; // for 8PSK

    /*! \brief the phase detector circuit for fourth-order loops.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_4(gr_complex sample) const; // for QPSK

    /*! \brief the phase detector circuit for second-order loops.
     *
     *  \param sample a complex sample
     *  \return the phase error
     */
    float phase_detector_2(gr_complex sample) const; // for BPSK


    /*! \brief the phase detector circuit for 8th-order PSK
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_snr_8(gr_complex sample) const; // for 8PSK

    /*! \brief the phase detector circuit for fourth-order
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_snr_4(gr_complex sample) const; // for QPSK

    /*! \brief the phase detector circuit for second-order
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample a complex sample
     *  \return the phase error
     */
    float phase_detector_snr_2(gr_complex sample) const; // for BPSK

    typedef float (costas_loop_cc_impl::*d_phase_detector_t)(gr_complex sample) const;
    static d_phase_detector_t choose_phase_detector(unsigned int order, bool use_snr);
    const d_phase_detector_t d_phase_detector;

public:
    costas_loop_cc_impl(float loop_bw, unsigned int order, bool use_snr = false);
    ~costas_loop_cc_impl();

    float error() const;

    void handle_set_noise(pmt::pmt_t msg);

    void setup_rpc();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H */
