/* -*- c++ -*- */
/*
 * Copyright 2003,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "freq_xlating_fir_filter_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
typename freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::sptr
freq_xlating_fir_filter<IN_T, OUT_T, TAP_T>::make(int decimation,
                                                  const std::vector<TAP_T>& taps,
                                                  double center_freq,
                                                  double sampling_freq)
{
    return gnuradio::make_block_sptr<freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>>(
        decimation, taps, center_freq, sampling_freq);
}

template <class IN_T, class OUT_T, class TAP_T>
freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::freq_xlating_fir_filter_impl(
    int decimation,
    const std::vector<TAP_T>& taps,
    double center_freq,
    double sampling_freq)
    : sync_decimator("freq_xlating_fir_filter<IN_T,OUT_T,TAP_T>",
                     io_signature::make(1, 1, sizeof(IN_T)),
                     io_signature::make(1, 1, sizeof(OUT_T)),
                     decimation),
      d_proto_taps(taps),
      d_composite_fir({}),
      d_center_freq(center_freq),
      d_prev_center_freq(0),
      d_sampling_freq(sampling_freq),
      d_updated(false),
      d_decim(decimation)
{
    this->set_history(this->d_proto_taps.size());
    this->build_composite_fir();

    this->message_port_register_in(pmt::mp("freq"));
    this->set_msg_handler(pmt::mp("freq"),
                          [this](pmt::pmt_t msg) { this->handle_set_center_freq(msg); });
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::build_composite_fir()
{
    std::vector<gr_complex> ctaps(d_proto_taps.size());

    // In order to avoid phase jumps during a retune, adjust the phase
    // of the rotator. Phase delay of a symmetric, odd length FIR is (N-1)/2.
    // Scale phase delay by delta omega to get the difference in phase response
    // caused by retuning. Subtract from the current rotator phase.

    gr_complex phase = d_r.phase();
    phase /= std::abs(phase);
    float delta_freq = d_center_freq - d_prev_center_freq;
    float delta_omega = 2.0 * GR_M_PI * delta_freq / d_sampling_freq;
    float delta_phase = -delta_omega * (d_proto_taps.size() - 1) / 2.0;
    phase *= exp(gr_complex(0, delta_phase));
    d_r.set_phase(phase);

    // The basic principle of this block is to perform:
    //    x(t) -> (mult by -fwT0) -> LPF -> decim -> y(t)
    // We switch things up here to:
    //    x(t) -> BPF -> decim -> (mult by fwT0*decim) -> y(t)
    // The BPF is the baseband filter (LPF) moved up to the
    // center frequency fwT0. We then apply a derotator
    // with -fwT0 to downshift the signal to baseband.

    float fwT0 = 2 * GR_M_PI * d_center_freq / d_sampling_freq;
    for (unsigned int i = 0; i < d_proto_taps.size(); i++) {
        ctaps[i] = d_proto_taps[i] * exp(gr_complex(0, i * fwT0));
    }

    d_composite_fir.set_taps(ctaps);
    d_r.set_phase_incr(exp(gr_complex(0, -fwT0 * this->decimation())));
    d_prev_center_freq = d_center_freq;
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::set_center_freq(double center_freq)
{
    d_center_freq = center_freq;
    d_updated = true;
}

template <class IN_T, class OUT_T, class TAP_T>
double freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::center_freq() const
{
    return d_center_freq;
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::set_taps(
    const std::vector<TAP_T>& taps)
{
    d_proto_taps = taps;
    d_updated = true;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_proto_taps;
}

template <class IN_T, class OUT_T, class TAP_T>
void freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::handle_set_center_freq(
    pmt::pmt_t msg)
{
    if (pmt::is_dict(msg) && pmt::dict_has_key(msg, pmt::intern("freq"))) {
        pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("freq"), pmt::PMT_NIL);
        if (pmt::is_real(x)) {
            double freq = pmt::to_double(x);
            set_center_freq(freq);
        }
    } else if (pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if (pmt::is_real(x)) {
            double freq = pmt::to_double(x);
            set_center_freq(freq);
        }
    }
}

template <class IN_T, class OUT_T, class TAP_T>
int freq_xlating_fir_filter_impl<IN_T, OUT_T, TAP_T>::work(
    int noutput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];

    // rebuild composite FIR if the center freq has changed
    if (d_updated) {
        this->set_history(d_proto_taps.size());
        build_composite_fir();
        d_updated = false;

        // Tell downstream items where the frequency change was applied
        this->add_item_tag(0,
                           this->nitems_written(0),
                           pmt::intern("freq"),
                           pmt::from_double(d_center_freq),
                           this->alias_pmt());
        return 0; // history requirements may have changed.
    }

    unsigned j = 0;
    for (int i = 0; i < noutput_items; i++) {
        out[i] = d_composite_fir.filter(&in[j]);
        j += d_decim;
    }

    // re-use of the same buffer as the input and output is safe for many volk functions
    // and faster than creating local temporary memory in the work function and doing an
    // extra copy. So out is used below as both the in and out params to the rotate
    // function.
    d_r.rotateN(out, out, noutput_items);

    return noutput_items;
}

template class freq_xlating_fir_filter<gr_complex, gr_complex, gr_complex>;
template class freq_xlating_fir_filter<gr_complex, gr_complex, float>;
template class freq_xlating_fir_filter<float, gr_complex, gr_complex>;
template class freq_xlating_fir_filter<float, gr_complex, float>;
template class freq_xlating_fir_filter<std::int16_t, gr_complex, float>;
template class freq_xlating_fir_filter<std::int16_t, gr_complex, gr_complex>;

} /* namespace filter */
} /* namespace gr */
