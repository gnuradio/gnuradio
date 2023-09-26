/* -*- c++ -*- */
/*
 * Copyright 2015,2019 Free Software Foundation, Inc.
 * Copyright 2023 Ettus Research, a National Instruments Brandh
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UHD_CMD_KEYS_H
#define INCLUDED_GR_UHD_CMD_KEYS_H

#include <gnuradio/uhd/api.h>
#include <pmt/pmt.h>

namespace gr {
namespace uhd {

GR_UHD_API const pmt::pmt_t cmd_chan_key();
GR_UHD_API const pmt::pmt_t cmd_gain_key();
GR_UHD_API const pmt::pmt_t cmd_power_key();
GR_UHD_API const pmt::pmt_t cmd_freq_key();
GR_UHD_API const pmt::pmt_t cmd_lo_offset_key();
GR_UHD_API const pmt::pmt_t cmd_tune_key();
GR_UHD_API const pmt::pmt_t cmd_mtune_key();
GR_UHD_API const pmt::pmt_t cmd_lo_freq_key();
GR_UHD_API const pmt::pmt_t cmd_dsp_freq_key();
GR_UHD_API const pmt::pmt_t cmd_rate_key();
GR_UHD_API const pmt::pmt_t cmd_bandwidth_key();
GR_UHD_API const pmt::pmt_t cmd_time_key();
GR_UHD_API const pmt::pmt_t cmd_mboard_key();
GR_UHD_API const pmt::pmt_t cmd_antenna_key();
GR_UHD_API const pmt::pmt_t cmd_direction_key();
GR_UHD_API const pmt::pmt_t cmd_tag_key();
GR_UHD_API const pmt::pmt_t cmd_gpio_key();
GR_UHD_API const pmt::pmt_t cmd_pc_clock_resync_key();
GR_UHD_API const pmt::pmt_t cmd_stream_cmd_key();

GR_UHD_API const pmt::pmt_t direction_rx();
GR_UHD_API const pmt::pmt_t direction_tx();

} // namespace uhd
} // namespace gr

#endif /* INCLUDED_GR_UHD_CMD_KEYS_H */
