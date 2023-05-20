/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, a National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_UHD_RFNOC_REPLAY_IMPL_H
#define INCLUDED_GR_UHD_RFNOC_REPLAY_IMPL_H

#include <gnuradio/uhd/rfnoc_replay.h>
#include <uhd/rfnoc/replay_block_control.hpp>

#include <pmt/pmt.h>

static const pmt::pmt_t REPLAY_IN_PORT_KEY = pmt::string_to_symbol("replay_in");

namespace gr {
namespace uhd {

class rfnoc_replay_impl : public rfnoc_replay
{
public:
    rfnoc_replay_impl(::uhd::rfnoc::noc_block_base::sptr block_ref);
    ~rfnoc_replay_impl() override;

    /*** API *****************************************************************/
    void
    record(const uint64_t offset, const uint64_t size, const size_t port = 0) override;
    void record_restart(const size_t port = 0) override;
    void play(const uint64_t offset,
              const uint64_t size,
              const size_t port,
              const ::uhd::time_spec_t time_spec,
              const bool repeat) override;
    void stop_playback(const size_t port = 0) override;
    void set_record_type(const std::string type, const size_t port = 0) override;
    void set_play_type(const std::string type, const size_t port = 0) override;
    void issue_stream_cmd(const ::uhd::stream_cmd_t& cmd, const size_t port = 0) override;

private:
    void _command_handler(pmt::pmt_t msg);

    ::uhd::rfnoc::replay_block_control::sptr d_wrapped_ref;
};

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_RFNOC_REPLAY_IMPL_H */
