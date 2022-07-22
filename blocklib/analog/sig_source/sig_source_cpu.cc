/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "sig_source_cpu.h"
#include "sig_source_cpu_gen.h"

#include <gnuradio/kernel/math/math.h>
#include <algorithm>

namespace gr {
namespace analog {

template <class T>
sig_source_cpu<T>::sig_source_cpu(const typename sig_source<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
    this->set_frequency(args.frequency);
    this->set_phase(args.phase);
}

template <typename T>
work_return_code_t sig_source_cpu<T>::work(work_io& wio)
{
    auto optr = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    T t;
    std::scoped_lock l(d_mutex);

    auto offset = pmtf::get_as<T>(*this->param_offset);
    auto ampl = pmtf::get_as<float>(*this->param_ampl);
    auto waveform = static_cast<waveform_t>(pmtf::get_as<int>(*this->param_waveform));

    switch (waveform) {
    case waveform_t::constant:
        t = (T)ampl + offset;
        std::fill_n(optr, noutput_items, t);
        break;

    case waveform_t::sin:
        d_nco.sin(optr, noutput_items, ampl);
        if (offset == 0)
            break;

        for (size_t i = 0; i < noutput_items; i++) {
            optr[i] += offset;
        }
        break;
    case waveform_t::cos:
        d_nco.cos(optr, noutput_items, ampl);
        if (offset == 0)
            break;

        for (size_t i = 0; i < noutput_items; i++) {
            optr[i] += offset;
        }
        break;

        /* The square wave is high from -PI to 0. */
    case waveform_t::square:
        t = (T)ampl + offset;
        for (size_t i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < 0)
                optr[i] = t;
            else
                optr[i] = offset;
            d_nco.step();
        }
        break;

        /* The triangle wave rises from -PI to 0 and falls from 0 to PI. */
    case waveform_t::triangle:
        for (size_t i = 0; i < noutput_items; i++) {
            double t = ampl * d_nco.get_phase() / GR_M_PI;
            if (d_nco.get_phase() < 0)
                optr[i] = static_cast<T>(t + ampl + offset);
            else
                optr[i] = static_cast<T>(-1 * t + ampl + offset);
            d_nco.step();
        }
        break;

        /* The saw tooth wave rises from -PI to PI. */
    case waveform_t::sawtooth:
        for (size_t i = 0; i < noutput_items; i++) {
            t = static_cast<T>(ampl * d_nco.get_phase() / (2 * GR_M_PI) + ampl / 2 +
                               offset);
            optr[i] = t;
            d_nco.step();
        }
        break;
    default:
        throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

template <>
work_return_code_t sig_source_cpu<gr_complex>::work(work_io& wio)

{
    auto optr = wio.outputs()[0].items<gr_complex>();
    auto noutput_items = wio.outputs()[0].n_items;

    gr_complex t;
    std::scoped_lock l(d_mutex);

    auto offset = pmtf::get_as<gr_complex>(*this->param_offset);
    auto ampl = pmtf::get_as<float>(*this->param_ampl);
    auto waveform = static_cast<waveform_t>(pmtf::get_as<int>(*this->param_waveform));

    switch (waveform) {
    case waveform_t::constant:
        t = (gr_complex)ampl + offset;
        std::fill_n(optr, noutput_items, t);
        break;

    case waveform_t::sin:
    case waveform_t::cos:
        d_nco.sincos(optr, noutput_items, ampl);
        if (offset == gr_complex(0, 0))
            break;

        for (size_t i = 0; i < noutput_items; i++) {
            optr[i] += offset;
        }
        break;

        /* Implements a real square wave high from -PI to 0.
         * The imaginary square wave leads by 90 deg.
         */
    case waveform_t::square:
        for (size_t i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2)
                optr[i] = gr_complex(ampl, 0) + offset;
            else if (d_nco.get_phase() < 0)
                optr[i] = gr_complex(ampl, ampl) + offset;
            else if (d_nco.get_phase() < GR_M_PI / 2)
                optr[i] = gr_complex(0, ampl) + offset;
            else
                optr[i] = offset;
            d_nco.step();
        }
        break;

        /* Implements a real triangle wave rising from -PI to 0 and
         * falling from 0 to PI. The imaginary triangle wave leads by
         * 90 deg.
         */
    case waveform_t::triangle:
        for (size_t i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2) {
                optr[i] = gr_complex(ampl * d_nco.get_phase() / GR_M_PI + ampl,
                                     -1 * ampl * d_nco.get_phase() / GR_M_PI - ampl / 2) +
                          offset;
            }
            else if (d_nco.get_phase() < 0) {
                optr[i] = gr_complex(ampl * d_nco.get_phase() / GR_M_PI + ampl,
                                     ampl * d_nco.get_phase() / GR_M_PI + ampl / 2) +
                          offset;
            }
            else if (d_nco.get_phase() < GR_M_PI / 2) {
                optr[i] = gr_complex(-1 * ampl * d_nco.get_phase() / GR_M_PI + ampl,
                                     ampl * d_nco.get_phase() / GR_M_PI + ampl / 2) +
                          offset;
            }
            else {
                optr[i] =
                    gr_complex(-1 * ampl * d_nco.get_phase() / GR_M_PI + ampl,
                               -1 * ampl * d_nco.get_phase() / GR_M_PI + 3 * ampl / 2) +
                    offset;
            }
            d_nco.step();
        }
        break;

        /* Implements a real saw tooth wave rising from -PI to PI.
         * The imaginary saw tooth wave leads by 90 deg.
         */
    case waveform_t::sawtooth:
        for (size_t i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2) {
                optr[i] =
                    gr_complex(ampl * d_nco.get_phase() / (2 * GR_M_PI) + ampl / 2,
                               ampl * d_nco.get_phase() / (2 * GR_M_PI) + 5 * ampl / 4) +
                    offset;
            }
            else {
                optr[i] =
                    gr_complex(ampl * d_nco.get_phase() / (2 * GR_M_PI) + ampl / 2,
                               ampl * d_nco.get_phase() / (2 * GR_M_PI) + ampl / 4) +
                    offset;
            }
            d_nco.step();
        }
        break;
    default:
        throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} /* namespace analog */
} /* namespace gr */
