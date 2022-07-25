/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "hackrf_source_cpu.h"
#include "hackrf_source_cpu_gen.h"

namespace gr {
namespace soapy {

template <class T>
hackrf_source_cpu<T>::hackrf_source_cpu(const typename hackrf_source<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
    d_soapy_source_block =
        soapy::source<T>::make({ "driver=hackrf", 1, args.dev_args, "", { "" }, { "" } });

    d_soapy_source_block->set_sample_rate(0, args.samp_rate);
    d_soapy_source_block->set_bandwidth(0, args.bandwidth);
    d_soapy_source_block->set_frequency(0, args.center_freq);
    d_soapy_source_block->set_gain(0, "AMP", args.amp);
    d_soapy_source_block->set_gain(0, "LNA", args.gain);
    d_soapy_source_block->set_gain(0, "VGA", args.vga);

    this->connect(d_soapy_source_block, 0, this->base(), 0);
}


template <class T>
void hackrf_source_cpu<T>::on_parameter_change(param_action_sptr action)
{
    // This will set the underlying PMT
    hier_block::on_parameter_change(action);

    // Pass the parameter changes along to the underlying block
    if (action->id() == hackrf_source<T>::id_samp_rate) {
        auto samp_rate = pmtf::get_as<float>(*this->param_samp_rate);
        d_soapy_source_block->set_sample_rate(0, samp_rate);
    }
    else if (action->id() == hackrf_source<T>::id_center_freq) {
        auto freq = pmtf::get_as<float>(*this->param_center_freq);
        d_soapy_source_block->set_frequency(0, freq);
    }
    else if (action->id() == hackrf_source<T>::id_gain) {
        auto gain = pmtf::get_as<float>(*this->param_gain);
        d_soapy_source_block->set_gain(0, "LNA", gain);
    }
}


} /* namespace soapy */
} /* namespace gr */
