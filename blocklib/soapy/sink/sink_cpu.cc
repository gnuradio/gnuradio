/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "sink_cpu.h"
#include "sink_cpu_gen.h"
#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Formats.h>
#include <volk/volk.h>

namespace gr {
namespace soapy {

template <>
sink_cpu<gr_complex>::sink_cpu(const typename sink<gr_complex>::block_args& args)
    : gr::block("soapy_sink"),
      sink<gr_complex>(args),
      block_impl(SOAPY_SDR_TX,
                 args.device,
                 SOAPY_SDR_CF32,
                 args.nchan,
                 args.dev_args,
                 args.stream_args,
                 args.tune_args,
                 args.other_settings)
{
}

template <class T>
work_return_t sink_cpu<T>::work(work_io& wio)
{
    int nin = wio.inputs()[0].n_items;
    long long int time_ns = 0;
    int nconsumed = 0;

    int nwrite = nin;
    int flags = 0;

    // TODO: optimization: add loop to handle portions of more than one burst
    // per call.

    // FIXME: Add back burst tag handling.  Requires some changes to tag/pmt

    int result = 0;
    if (nwrite != 0) {
        // No command handlers while writing
        std::lock_guard<std::mutex> l(d_device_mutex);
        result = d_device->writeStream(
            d_stream, wio.all_input_ptrs().data(), nwrite, flags, time_ns);
    }

    if (result >= 0) {
        nconsumed += result;
        // if (d_burst_remaining > 0) {
        //     d_burst_remaining -= result;
        // }
    }
    else if (result == SOAPY_SDR_UNDERFLOW) {
        std::cerr << "sU" << std::flush;
    }
    else {
        d_logger->warn("Soapy sink error: {:s}", SoapySDR::errToStr(result));
    }

    wio.consume_each(nconsumed);
    return work_return_t::OK;
}


} /* namespace soapy */
} /* namespace gr */
