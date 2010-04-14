/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2010 Free Software Foundation, Inc.
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
#include "control.h"
#include "ring.h"
#include <string>

#define MAX_SUBPKT_LEN 252

namespace usrp2 {

  class eth_buffer;
  class pktfilter;
  class usrp2_thread;
  class usrp2_tune_result;
  class pending_reply;
  class ring;

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

  class usrp2::impl : private data_handler
  {
    static const size_t NRIDS = 256;
    static const size_t NCHANS = 32;

    eth_buffer    *d_eth_buf;
    std::string    d_interface_name;
    pktfilter     *d_pf;
    std::string    d_addr;       // FIXME: use u2_mac_addr_t instead

    boost::thread_group d_rx_tg;
    volatile bool  d_bg_running; // TODO: multistate if needed

    int            d_rx_seqno;
    int            d_tx_seqno;
    int            d_next_rid;
    unsigned int   d_num_rx_frames;
    unsigned int   d_num_rx_missing;
    unsigned int   d_num_rx_overruns;
    unsigned int   d_num_rx_bytes;

    unsigned int   d_num_enqueued;
    gruel::mutex   d_enqueued_mutex;
    gruel::condition_variable d_bg_pending_cond;

    // all pending_replies are stack allocated, thus no possibility of leaking these
    pending_reply *d_pending_replies[NRIDS]; // indexed by 8-bit reply id

    std::vector<ring_sptr>   d_channel_rings; // indexed by 5-bit channel number
    gruel::mutex   d_channel_rings_mutex;

    db_info	   d_tx_db_info;
    db_info	   d_rx_db_info;

    int		   d_tx_interp;		// shadow tx interp
    int		   d_rx_decim;		// shadow rx decim

    bool	   d_dont_enqueue;

    void inc_enqueued() {
      gruel::scoped_lock l(d_enqueued_mutex);
      d_num_enqueued++;
    }

    void dec_enqueued() {
      gruel::scoped_lock l(d_enqueued_mutex);
      if (--d_num_enqueued == 0)
        d_bg_pending_cond.notify_one();
    }

    static bool parse_mac_addr(const std::string &s, u2_mac_addr_t *p);
    void init_et_hdrs(u2_eth_packet_t *p, const std::string &dst);
    void init_etf_hdrs(u2_eth_packet_t *p, const std::string &dst,
		       int word0_flags, int chan, uint32_t timestamp);
    void start_bg();
    void stop_bg();
    void init_config_rx_v2_cmd(op_config_rx_v2_cmd *cmd);
    void init_config_tx_v2_cmd(op_config_tx_v2_cmd *cmd);
    bool transmit_cmd_and_wait(void *cmd, size_t len, pending_reply *p, double secs=0.0);
    bool transmit_cmd(void *cmd, size_t len);
    virtual data_handler::result operator()(const void *base, size_t len);
    data_handler::result handle_control_packet(const void *base, size_t len);
    data_handler::result handle_data_packet(const void *base, size_t len);
    bool dboard_info();
    bool reset_db();

  public:
    impl(const std::string &ifc, props *p, size_t rx_bufsize);
    ~impl();

    std::string mac_addr() const { return d_addr; } // FIXME: convert from u2_mac_addr_t
    std::string interface_name() const { return d_interface_name; }

    // Rx

    bool set_rx_antenna(int ant);
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
    bool start_rx_streaming(unsigned int channel, unsigned int items_per_frame);
    bool start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time);
    bool sync_and_start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time);
    bool rx_samples(unsigned int channel, rx_sample_handler *handler);
    bool flush_rx_samples(unsigned int channel);
    bool stop_rx_streaming(unsigned int channel);
    unsigned int rx_overruns() const { return d_num_rx_overruns; }
    unsigned int rx_missing() const { return d_num_rx_missing; }

    // Tx

    bool set_tx_antenna(int ant);
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

    bool tx_32fc(unsigned int channel,
		 const std::complex<float> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata);

    bool tx_16sc(unsigned int channel,
		 const std::complex<int16_t> *samples,
		 size_t nsamples,
		 const tx_metadata *metadata);

    bool tx_raw(unsigned int channel,
		const uint32_t *items,
		size_t nitems,
		const tx_metadata *metadata);

    // misc

    bool config_mimo(int flags);
    bool fpga_master_clock_freq(long *freq);
    bool adc_rate(long *rate);
    bool dac_rate(long *rate);
    bool tx_daughterboard_id(int *dbid);
    bool rx_daughterboard_id(int *dbid);

    // low level

    bool burn_mac_addr(const std::string &new_addr);
    bool sync_to_pps();
    bool sync_every_pps(bool enable);
    std::vector<uint32_t> peek32(uint32_t addr, uint32_t words);
    bool poke32(uint32_t addr, const std::vector<uint32_t> &data);

    // Receive thread, need to be public for boost::bind
    void bg_loop();
  };

} // namespace usrp2

#endif /* INCLUDED_USRP2_IMPL_H */
