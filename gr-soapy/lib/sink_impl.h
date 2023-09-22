/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_SOAPY_GR_SINK_IMPL_H
#define INCLUDED_SOAPY_GR_SINK_IMPL_H

#include <functional>

#include "block_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/soapy/sink.h>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Version.hpp>

namespace gr {
namespace soapy {

/*!
 * \brief Sink block implementation for SDR devices.
 */

class sink_impl : public sink, public block_impl
{
public:
    sink_impl(const std::string& device,
              const std::string& type,
              size_t nchan,
              const std::string& dev_args,
              const std::string& stream_args,
              const std::vector<std::string>& tune_args,
              const std::vector<std::string>& other_settings);
    ~sink_impl() override{};

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_length_tag_name(const std::string& length_tag_name) override;

private:
    pmt::pmt_t d_length_tag_key = pmt::get_PMT_NIL();
    long d_burst_remaining;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_SOAPY_GR_SINK_IMPL_H */
