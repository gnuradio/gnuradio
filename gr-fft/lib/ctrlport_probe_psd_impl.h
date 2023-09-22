/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H
#define INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H

#include <gnuradio/fft/ctrlport_probe_psd.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <condition_variable>
#include <mutex>

namespace gr {
namespace fft {

class ctrlport_probe_psd_impl : public ctrlport_probe_psd
{
private:
    std::string d_id;
    std::string d_desc;
    size_t d_len;
    std::mutex mutex_buffer;
    mutable std::mutex mutex_notify;
    std::condition_variable condition_buffer_ready;

    std::vector<gr_complex> d_buffer;
    gr::fft::fft_complex_fwd d_fft;

public:
    ctrlport_probe_psd_impl(const std::string& id, const std::string& desc, int len);
    ~ctrlport_probe_psd_impl();

    void setup_rpc() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    std::vector<gr_complex> get() override;

    void set_length(int len) override;
    int length() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace fft
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE_PSD_IMPL_H */
