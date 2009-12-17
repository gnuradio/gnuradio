/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_USRP2_IMPL_H
#define INCLUDED_USRP2_IMPL_H

#include <usrp2/usrp2.h>
#include <usrp2/data_handler.h>
#include <usrp2_eth_packet.h>
#include <gruel/thread.h>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "control.h"
#include "transport.h"
#include <string>

#define MAX_SUBPKT_LEN 252

#define dimof(_x) (sizeof(_x)/sizeof(_x[0]))

namespace usrp2 {

  class usrp2_tune_result;
  class pending_reply;

  //! High-level d'board info
  struct db_info {
    int		dbid;
    double	freq_min;		// Hz
    double	freq_max;		// Hz
    double	gain_min;		// dB
    double	gain_max;		// dB
    double	gain_step_size;		// dB

    db_info() : dbid(-1), freq_min(0), freq_max(0),
		gain_min(0), gain_max(0), gain_step_size(0) {}
  };

  class usrp2::impl
  {
    static const size_t NRIDS = 256;

    int            d_next_rid;
    int            d_tx_pkt_cnt;

    // all pending_replies are stack allocated, thus no possibility of leaking these
    pending_reply *d_pending_replies[NRIDS]; // indexed by 8-bit reply id

    db_info	   d_tx_db_info;
    db_info	   d_rx_db_info;

    int		   d_tx_interp;		// shadow tx interp 
    int		   d_rx_decim;		// shadow rx decim

    void init_config_rx_v2_cmd(op_config_rx_v2_cmd *cmd);
    void init_config_tx_v2_cmd(op_config_tx_v2_cmd *cmd);
    bool transmit_cmd_and_wait(void *cmd, size_t len, pending_reply *p, double secs=0.0);
    bool transmit_cmd(void *cmd, size_t len);
    bool dboard_info();
    bool reset_db();

    void ctrl_thread_loop(void);
    boost::thread *d_ctrl_thread;
    bool d_ctrl_thread_running;
    transport::sptr d_ctrl_transport;
    transport::sptr d_data_transport;

  public:
    impl(transport::sptr data_transport, transport::sptr ctrl_transport);
    ~impl();

    // Rx

    bool set_rx_gain(double gain);
    double rx_gain_min() { return d_rx_db_info.gain_min; }
    double rx_gain_max() { return d_rx_db_info.gain_max; }
    double rx_gain_db_per_step() { return d_rx_db_info.gain_step_size; }
    bool set_rx_lo_offset(double frequency);
    bool set_rx_center_freq(double frequency, tune_result *result);
    double rx_freq_min() { return d_rx_db_info.freq_min; }
    double rx_freq_max() { return d_rx_db_info.freq_max; }
    bool set_rx_decim(int decimation_factor);
    int rx_decim() { return d_rx_decim; }
    bool set_rx_scale_iq(int scale_i, int scale_q);
    bool set_gpio_ddr(int bank, uint16_t value, uint16_t mask);
    bool set_gpio_sels(int bank, std::string src);
    bool enable_gpio_streaming(int bank, int enable);
    bool write_gpio(int bank, uint16_t value, uint16_t mask);
    bool read_gpio(int bank, uint16_t *value);
    bool start_rx_streaming(unsigned int items_per_frame);
    bool rx_samples(vrt::rx_packet_handler *handler);
    bool stop_rx_streaming();

    // Tx

    bool set_tx_gain(double gain);
    double tx_gain_min() { return d_tx_db_info.gain_min; }
    double tx_gain_max() { return d_tx_db_info.gain_max; }
    double tx_gain_db_per_step() { return d_tx_db_info.gain_step_size; }
    bool set_tx_lo_offset(double frequency);
    bool set_tx_center_freq(double frequency, tune_result *result);
    double tx_freq_min() { return d_tx_db_info.freq_min; }
    double tx_freq_max() { return d_tx_db_info.freq_max; }
    bool set_tx_interp(int interpolation_factor);
    int tx_interp() { return d_tx_interp; }
    void default_tx_scale_iq(int interpolation_factor, int *scale_i, int *scale_q);
    bool set_tx_scale_iq(int scale_i, int scale_q);

    bool tx_32fc(
		 const std::complex<float> *samples,
		 size_t nsamples,
		 const vrt::expanded_header *hdr);

    bool tx_16sc(
		 const std::complex<int16_t> *samples,
		 size_t nsamples,
		 const vrt::expanded_header *hdr);

    bool tx_raw(
		const uint32_t *items,
		size_t nitems,
		const vrt::expanded_header *hdr);

    // misc

    bool config_mimo(int flags);
    bool fpga_master_clock_freq(long *freq);
    bool adc_rate(long *rate);
    bool dac_rate(long *rate);
    bool tx_daughterboard_id(int *dbid);
    bool rx_daughterboard_id(int *dbid);

    // low level

    bool burn_mac_addr(u2_mac_addr_t *new_mac);
    bool sync_to_pps();
    bool sync_every_pps(bool enable);
    std::vector<uint32_t> peek32(uint32_t addr, uint32_t words);
    bool poke32(uint32_t addr, const std::vector<uint32_t> &data);
  };
  
} // namespace usrp2

#endif /* INCLUDED_USRP2_IMPL_H */
