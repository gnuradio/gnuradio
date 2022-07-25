/* -*- c++ -*- */
/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/soapy/hackrf_source.h>
#include <gnuradio/soapy/source.h>

namespace gr {
namespace soapy {

template <class T>
class hackrf_source_cpu : public hackrf_source<T>
{
public:
    hackrf_source_cpu(const typename hackrf_source<T>::block_args& args);

private:
    std::shared_ptr<soapy::source<T>> d_soapy_source_block;
    void on_parameter_change(param_action_sptr action) override;
};


} // namespace soapy
} // namespace gr
