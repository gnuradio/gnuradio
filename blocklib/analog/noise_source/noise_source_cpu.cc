/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "noise_source_cpu.h"
#include "noise_source_cpu_gen.h"

namespace gr {
namespace analog {

template <class T>
noise_source_cpu<T>::noise_source_cpu(const typename noise_source<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T), d_rng(args.seed)
{
}

template <>
noise_source_cpu<gr_complex>::noise_source_cpu(
    const typename noise_source<gr_complex>::block_args& args)
    : INHERITED_CONSTRUCTORS(gr_complex), d_rng(args.seed)
{
    // param_amplitude gets set in the INHERITED_CONSTRUCTORS from the noise_source
    // autogen class but for complex, needs the 1/sqrt(2) factor
    *param_amplitude = args.amplitude / sqrtf(2.0f);
}

template <class T>
work_return_code_t noise_source_cpu<T>::work(work_io& wio)

{
    auto out = wio.outputs()[0].items<T>();
    auto noutput_items = wio.outputs()[0].n_items;

    auto type = pmtf::get_as<int>(*this->param_type);
    auto ampl = pmtf::get_as<float>(*this->param_amplitude);

    switch (static_cast<noise_t>(type)) {
    case noise_t::uniform:
        for (size_t i = 0; i < noutput_items; i++) {
            out[i] = static_cast<T>(ampl * ((d_rng.ran1() * 2.0) - 1.0));
        }
        break;

    case noise_t::gaussian:
        for (size_t i = 0; i < noutput_items; i++) {
            out[i] = static_cast<T>(ampl * d_rng.gasdev());
        }
        break;

    case noise_t::laplacian:
        for (size_t i = 0; i < noutput_items; i++) {
            out[i] = static_cast<T>(ampl * d_rng.laplacian());
        }
        break;

    case noise_t::impulse: // FIXME changeable impulse settings
        for (size_t i = 0; i < noutput_items; i++) {
            out[i] = static_cast<T>(ampl * d_rng.impulse(9));
        }
        break;
    default:
        throw std::runtime_error("invalid type");
    }

    wio.produce_each(noutput_items);
    return work_return_code_t::WORK_OK;
}

} /* namespace analog */
} /* namespace gr */
