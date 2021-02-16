/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_SOURCE_IMPL_H
#define INCLUDED_GR_SOAPY_SOURCE_IMPL_H

#include <functional>

#include "block_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/soapy/source.h>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Version.hpp>

namespace gr {
namespace soapy {

/*!
 * \brief Source block implementation for SDR devices.
 */

class source_impl : public source, public block_impl
{
public:
    source_impl(const std::string& device,
                const std::string& type,
                size_t nchan,
                const std::string& dev_args,
                const std::string& stream_args,
                const std::vector<std::string>& tune_args,
                const std::vector<std::string>& other_settings);
    ~source_impl() override{};

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_SOURCE_IMPL_H */
