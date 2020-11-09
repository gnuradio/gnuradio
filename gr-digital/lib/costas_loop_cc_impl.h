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
    bool d_use_snr;
    int d_order;

    /*! \brief the phase detector circuit for 8th-order PSK loops.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_8(gr_complex sample) const // for 8PSK
    {
        /* This technique splits the 8PSK constellation into 2 squashed
   QPSK constellations, one when I is larger than Q and one
   where Q is larger than I. The error is then calculated
   proportionally to these squashed constellations by the const
   K = sqrt(2)-1.

   The signal magnitude must be > 1 or K will incorrectly bias
   the error value.

   Ref: Z. Huang, Z. Yi, M. Zhang, K. Wang, "8PSK demodulation for
   new generation DVB-S2", IEEE Proc. Int. Conf. Communications,
   Circuits and Systems, Vol. 2, pp. 1447 - 1450, 2004.
         */

        const float K = (sqrtf(2.0) - 1);
        if (fabsf(sample.real()) >= fabsf(sample.imag())) {
            return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() -
                    (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real() * K);
        } else {
            return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() * K -
                    (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real());
        }
    };

    /*! \brief the phase detector circuit for fourth-order loops.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_4(gr_complex sample) const // for QPSK
    {
        return ((sample.real() > 0.0f ? 1.0f : -1.0f) * sample.imag() -
                (sample.imag() > 0.0f ? 1.0f : -1.0f) * sample.real());
    };

    /*! \brief the phase detector circuit for second-order loops.
     *
     *  \param sample a complex sample
     *  \return the phase error
     */
    float phase_detector_2(gr_complex sample) const // for BPSK
    {
        return (sample.real() * sample.imag());
    }

    /*! \brief the phase detector circuit for 8th-order PSK
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_snr_8(gr_complex sample) const // for 8PSK
    {
        const float K = (sqrtf(2.0) - 1.0);
        const float snr = std::norm(sample) / d_noise;
        if (fabsf(sample.real()) >= fabsf(sample.imag())) {
            return ((blocks::tanhf_lut(snr * sample.real()) * sample.imag()) -
                    (blocks::tanhf_lut(snr * sample.imag()) * sample.real() * K));
        } else {
            return ((blocks::tanhf_lut(snr * sample.real()) * sample.imag() * K) -
                    (blocks::tanhf_lut(snr * sample.imag()) * sample.real()));
        }
    };

    /*! \brief the phase detector circuit for fourth-order
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample complex sample
     *  \return the phase error
     */
    float phase_detector_snr_4(gr_complex sample) const // for QPSK
    {
        const float snr = std::norm(sample) / d_noise;
        return ((blocks::tanhf_lut(snr * sample.real()) * sample.imag()) -
                (blocks::tanhf_lut(snr * sample.imag()) * sample.real()));
    };

    /*! \brief the phase detector circuit for second-order
     *  loops. Uses tanh instead of slicing and the noise estimate
     *  from the message port to estimated SNR of the samples.
     *
     *  \param sample a complex sample
     *  \return the phase error
     */
    float phase_detector_snr_2(gr_complex sample) const // for BPSK
    {
        const float snr = std::norm(sample) / d_noise;
        return blocks::tanhf_lut(snr * sample.real()) * sample.imag();
    };

public:
    costas_loop_cc_impl(float loop_bw, unsigned int order, bool use_snr = false);
    ~costas_loop_cc_impl() override;

    float error() const override { return d_error; };

    void handle_set_noise(pmt::pmt_t msg);

    void setup_rpc() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_COSTAS_LOOP_CC_IMPL_H */
