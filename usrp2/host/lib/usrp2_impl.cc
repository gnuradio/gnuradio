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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include <usrp2/tune_result.h>
#include <usrp2/copiers.h>
#include <gruel/inet.h>
#include <gruel/realtime.h>
#include <boost/bind.hpp>
#include <usrp2_types.h>
#include "usrp2_impl.h"
#include "eth_buffer.h"
#include "pktfilter.h"
#include "control.h"
#include "ring.h"
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#define USRP2_IMPL_DEBUG 0
#if USRP2_IMPL_DEBUG
#define DEBUG_LOG(x) ::write(2, x, 1)
#else
#define DEBUG_LOG(x)
#endif

static const int DEFAULT_RX_SCALE = 1024;

namespace usrp2 {

  static const double DEF_CMD_TIMEOUT = 0.1;

  std::string
  opcode_to_string(int opcode)
  {
    switch(opcode){
    case OP_EOP: return "OP_EOP";
    case OP_ID: return "OP_ID";
    case OP_ID_REPLY: return "OP_ID_REPLY";
    case OP_BURN_MAC_ADDR: return "OP_BURN_MAC_ADDR";
    case OP_READ_TIME: return "OP_READ_TIME";
    case OP_READ_TIME_REPLY: return "OP_READ_TIME_REPLY";
    case OP_CONFIG_RX_V2: return "OP_CONFIG_RX_V2";
    case OP_CONFIG_RX_REPLY_V2: return "OP_CONFIG_RX_REPLY_V2";
    case OP_CONFIG_TX_V2: return "OP_CONFIG_TX_V2";
    case OP_CONFIG_TX_REPLY_V2: return "OP_CONFIG_TX_REPLY_V2";
    case OP_START_RX_STREAMING: return "OP_START_RX_STREAMING";
    case OP_STOP_RX: return "OP_STOP_RX";
    case OP_CONFIG_MIMO: return "OP_CONFIG_MIMO";
    case OP_DBOARD_INFO: return "OP_DBOARD_INFO";
    case OP_DBOARD_INFO_REPLY: return "OP_DBOARD_INFO_REPLY";
    case OP_SYNC_TO_PPS: return "OP_SYNC_TO_PPS";
    case OP_PEEK: return "OP_PEEK";
    case OP_PEEK_REPLY: return "OP_PEEK_REPLY";
    case OP_SET_TX_LO_OFFSET: return "OP_SET_TX_LO_OFFSET";
    case OP_SET_TX_LO_OFFSET_REPLY: return "OP_SET_TX_LO_OFFSET_REPLY";
    case OP_SET_RX_LO_OFFSET: return "OP_SET_RX_LO_OFFSET";
    case OP_SET_RX_LO_OFFSET_REPLY: return "OP_SET_RX_LO_OFFSET_REPLY";
    case OP_SYNC_EVERY_PPS: return "OP_SYNC_EVERY_PPS";
    case OP_SYNC_EVERY_PPS_REPLY: return "OP_SYNC_EVERY_PPS_REPLY";

    default:
      char buf[64];
      snprintf(buf, sizeof(buf), "<unknown opcode: %d>", opcode);
      return buf;
    }
  }


  /*!
   * \param p points to fixed header
   * \param payload_len_in_bytes is length of the fixed hdr and the payload
   * \param[out] items is set to point to the first uint32 item in the payload
   * \param[out] nitems is set to the number of uint32 items in the payload
   * \param[out] md is filled in with the parsed metadata from the frame.
   */
  static bool
  parse_rx_metadata(void *p, size_t payload_len_in_bytes,
		    uint32_t **items, size_t *nitems_in_uint32s, rx_metadata *md)
  {
    if (payload_len_in_bytes < sizeof(u2_fixed_hdr_t))	// invalid format
      return false;

    // FIXME deal with the fact that (p % 4) == 2
    //assert((((uintptr_t) p) % 4) == 0);		// must be 4-byte aligned

    u2_fixed_hdr_t *fh = static_cast<u2_fixed_hdr_t *>(p);

    // FIXME unaligned loads!
    md->word0 = u2p_word0(fh);
    md->timestamp = u2p_timestamp(fh);

    // FIXME when we've got more info
    // md->start_of_burst = (md->word0 & XXX) != 0;
    // md->end_of_burst =   (md->word0 & XXX) != 0;
    // md->rx_overrun =     (md->word0 & XXX) != 0;
    md->start_of_burst = 0;
    md->end_of_burst =   0;
    md->rx_overrun =     0;

    *items = (uint32_t *)(&fh[1]);
    size_t nbytes = payload_len_in_bytes - sizeof(u2_fixed_hdr_t);
    assert((nbytes % sizeof(uint32_t)) == 0);
    *nitems_in_uint32s = nbytes / sizeof(uint32_t);

    return true;
  }


  usrp2::impl::impl(const std::string &ifc, props *p, size_t rx_bufsize)
    : d_eth_buf(new eth_buffer(rx_bufsize)), d_interface_name(ifc), d_pf(0),
      d_bg_running(false), d_rx_seqno(-1), d_tx_seqno(0), d_next_rid(0),
      d_num_rx_frames(0), d_num_rx_missing(0), d_num_rx_overruns(0), d_num_rx_bytes(0),
      d_num_enqueued(0), d_enqueued_mutex(), d_bg_pending_cond(),
      d_channel_rings(NCHANS), d_tx_interp(0), d_rx_decim(0), d_dont_enqueue(true)
  {
    if (!d_eth_buf->open(ifc, htons(U2_ETHERTYPE)))
      throw std::runtime_error("Unable to register USRP2 protocol");

    d_addr = p->addr;

    // Create a packet filter for U2_ETHERTYPE packets sourced from target USRP2
    u2_mac_addr_t usrp_mac;
    parse_mac_addr(d_addr, &usrp_mac);
    d_pf = pktfilter::make_ethertype_inbound_target(U2_ETHERTYPE, (const unsigned char*)&(usrp_mac.addr));
    if (!d_pf || !d_eth_buf->attach_pktfilter(d_pf))
      throw std::runtime_error("Unable to attach packet filter.");

    if (USRP2_IMPL_DEBUG)
      std::cerr << "usrp2 constructor: using USRP2 at " << d_addr << std::endl;

    memset(d_pending_replies, 0, sizeof(d_pending_replies));

    // Kick off receive thread
    start_bg();

    // In case the USRP2 was left streaming RX
    // FIXME: only one channel right now
    stop_rx_streaming(0);

    if (!dboard_info())		// we're hosed
      throw std::runtime_error("Unable to retrieve daughterboard info");

    if (0){
      int dbid;

      tx_daughterboard_id(&dbid);
      fprintf(stderr, "Tx dboard 0x%x\n", dbid);
      fprintf(stderr, "  freq_min = %g\n", tx_freq_min());
      fprintf(stderr, "  freq_max = %g\n", tx_freq_max());
      fprintf(stderr, "  gain_min = %g\n", tx_gain_min());
      fprintf(stderr, "  gain_max = %g\n", tx_gain_max());
      fprintf(stderr, "  gain_db_per_step = %g\n", tx_gain_db_per_step());

      rx_daughterboard_id(&dbid);
      fprintf(stderr, "Rx dboard 0x%x\n", dbid);
      fprintf(stderr, "  freq_min = %g\n", rx_freq_min());
      fprintf(stderr, "  freq_max = %g\n", rx_freq_max());
      fprintf(stderr, "  gain_min = %g\n", rx_gain_min());
      fprintf(stderr, "  gain_max = %g\n", rx_gain_max());
      fprintf(stderr, "  gain_db_per_step = %g\n", rx_gain_db_per_step());
    }

    // Ensure any custom values in hardware are cleared
    if (!reset_db())
      std::cerr << "usrp2::ctor reset_db failed\n";

    // default gains to mid point
    if (!set_tx_gain((tx_gain_min() + tx_gain_max()) / 2))
      std::cerr << "usrp2::ctor set_tx_gain failed\n";

    if (!set_rx_gain((rx_gain_min() + rx_gain_max()) / 2))
      std::cerr << "usrp2::ctor set_rx_gain failed\n";

    // default interp and decim
    if (!set_tx_interp(12))
      std::cerr << "usrp2::ctor set_tx_interp failed\n";

    if (!set_rx_decim(12))
      std::cerr << "usrp2::ctor set_rx_decim failed\n";

    // set workable defaults for scaling
    if (!set_rx_scale_iq(DEFAULT_RX_SCALE, DEFAULT_RX_SCALE))
      std::cerr << "usrp2::ctor set_rx_scale_iq failed\n";
  }

  usrp2::impl::~impl()
  {
    stop_bg();
    delete d_pf;
    d_eth_buf->close();
    delete d_eth_buf;

    if (USRP2_IMPL_DEBUG) {
      std::cerr << std::endl
                << "usrp2 destructor: received " << d_num_rx_frames
		<< " frames, with " << d_num_rx_missing << " lost ("
		<< (d_num_rx_frames == 0 ? 0 : (int)(100.0*d_num_rx_missing/d_num_rx_frames))
		<< "%), totaling " << d_num_rx_bytes
		<< " bytes" << std::endl;
    }
  }

  bool
  usrp2::impl::parse_mac_addr(const std::string &s, u2_mac_addr_t *p)
  {
    p->addr[0] = 0x00;		// Matt's IAB
    p->addr[1] = 0x50;
    p->addr[2] = 0xC2;
    p->addr[3] = 0x85;
    p->addr[4] = 0x30;
    p->addr[5] = 0x00;

    int len = s.size();

    switch (len){

    case 5:
      return sscanf(s.c_str(), "%hhx:%hhx", &p->addr[4], &p->addr[5]) == 2;

    case 17:
      return sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		    &p->addr[0], &p->addr[1], &p->addr[2],
		    &p->addr[3], &p->addr[4], &p->addr[5]) == 6;
    default:
      return false;
    }
  }

  void
  usrp2::impl::init_et_hdrs(u2_eth_packet_t *p, const std::string &dst)
  {
    p->ehdr.ethertype = htons(U2_ETHERTYPE);
    parse_mac_addr(dst, &p->ehdr.dst);
    memcpy(&p->ehdr.src, d_eth_buf->mac(), 6);
    p->thdr.flags = 0; // FIXME transport header values?
    p->thdr.seqno = d_tx_seqno++;
    p->thdr.ack = 0;
  }

  void
  usrp2::impl::init_etf_hdrs(u2_eth_packet_t *p, const std::string &dst,
			     int word0_flags, int chan, uint32_t timestamp)
  {
    init_et_hdrs(p, dst);
    u2p_set_word0(&p->fixed, word0_flags, chan);
    u2p_set_timestamp(&p->fixed, timestamp);

    if (chan == CONTROL_CHAN) { // no sequence numbers, back it out
      p->thdr.seqno = 0;
      d_tx_seqno--;
    }
  }

  void
  usrp2::impl::init_config_rx_v2_cmd(op_config_rx_v2_cmd *cmd)
  {
    memset(cmd, 0, sizeof(*cmd));
    init_etf_hdrs(&cmd->h, d_addr, 0, CONTROL_CHAN, -1);
    cmd->op.opcode = OP_CONFIG_RX_V2;
    cmd->op.len = sizeof(cmd->op);
    cmd->op.rid = d_next_rid++;
    cmd->eop.opcode = OP_EOP;
    cmd->eop.len = sizeof(cmd->eop);
  }

  void
  usrp2::impl::init_config_tx_v2_cmd(op_config_tx_v2_cmd *cmd)
  {
    memset(cmd, 0, sizeof(*cmd));
    init_etf_hdrs(&cmd->h, d_addr, 0, CONTROL_CHAN, -1);
    cmd->op.opcode = OP_CONFIG_TX_V2;
    cmd->op.len = sizeof(cmd->op);
    cmd->op.rid = d_next_rid++;
    cmd->eop.opcode = OP_EOP;
    cmd->eop.len = sizeof(cmd->eop);
  }


  bool
  usrp2::impl::transmit_cmd(void *cmd_, size_t len_)
  {
    const void *cmd = cmd_;
    int len = len_;
    unsigned char tmp[64];

    if (len_ < 64){		// pad to minimum ethernet frame size
      memset(tmp, 0, sizeof(tmp));
      memcpy(tmp, cmd_, len_);
      cmd = tmp;
      len = sizeof(tmp);
    }

    return d_eth_buf->tx_frame(cmd, len) == eth_buffer::EB_OK;
  }

  bool
  usrp2::impl::transmit_cmd_and_wait(void *cmd, size_t len, pending_reply *p, double secs)
  {
    d_pending_replies[p->rid()] = p;

    if (!transmit_cmd(cmd, len)){
      d_pending_replies[p->rid()] = 0;
      return false;
    }

    int res = p->wait_for_completion(secs);
    d_pending_replies[p->rid()] = 0;
    return res == 1;
  }

  // ----------------------------------------------------------------
  //        Background loop: received packet demuxing
  // ----------------------------------------------------------------

  void
  usrp2::impl::start_bg()
  {
    d_rx_tg.create_thread(boost::bind(&usrp2::impl::bg_loop, this));
  }

  void
  usrp2::impl::stop_bg()
  {
    d_bg_running = false;
    d_bg_pending_cond.notify_one(); // FIXME: check if needed
    d_rx_tg.join_all();
  }

  void
  usrp2::impl::bg_loop()
  {
    gruel::enable_realtime_scheduling();

    d_bg_running = true;
    while(d_bg_running) {
      DEBUG_LOG(":");
      // Receive available frames from ethernet buffer.  Handler will
      // process control frames, enqueue data packets in channel
      // rings, and signal blocked API threads
      int res = d_eth_buf->rx_frames(this, 100); // FIXME magic timeout
      if (res == eth_buffer::EB_ERROR)
	break;

      // Wait for user API thread(s) to process all enqueued packets.
      // The channel ring thread that decrements d_num_enqueued to zero
      // will signal this thread to continue.
      {
        gruel::scoped_lock l(d_enqueued_mutex);
        while(d_num_enqueued > 0 && d_bg_running)
	  d_bg_pending_cond.wait(l);
      }
    }
    d_bg_running = false;
  }

  //
  // passed to eth_buffer::rx_frames
  //
  data_handler::result
  usrp2::impl::operator()(const void *base, size_t len)
  {
    u2_eth_samples_t *pkt = (u2_eth_samples_t *)base;

    // FIXME unaligned load!
    int chan = u2p_chan(&pkt->hdrs.fixed);

    if (chan == CONTROL_CHAN) {	        // control packets
      DEBUG_LOG("c");
      return handle_control_packet(base, len);
    }
    else {				// data packets

      if (d_dont_enqueue)		// toss packet
	return data_handler::RELEASE;

      return handle_data_packet(base, len);
    }

    // not reached
  }

  data_handler::result
  usrp2::impl::handle_control_packet(const void *base, size_t len)
  {
    // point to beginning of payload (subpackets)
    unsigned char *p = (unsigned char *)base + sizeof(u2_eth_packet_t);

    // FIXME (p % 4) == 2.  Not good.  Must watch for unaligned loads.

    // FIXME iterate over payload, handling more than a single subpacket.

    int opcode = p[0];
    unsigned int oplen = p[1];
    unsigned int rid = p[2];

    pending_reply *rp = d_pending_replies[rid];
    if (rp) {
      unsigned int buflen = rp->len();
      if (oplen != buflen) {
	std::cerr << "usrp2: mismatched command reply length (expected: "
		  << buflen << " got: " << oplen << "). "
		  << "op = " << opcode_to_string(opcode) << std::endl;
      }

      // Copy reply into caller's buffer
      memcpy(rp->buffer(), p, std::min(oplen, buflen));
      rp->notify_completion();
      d_pending_replies[rid] = 0;
      return data_handler::RELEASE;
    }

    // TODO: handle unsolicited, USRP2 initiated, or late replies
    DEBUG_LOG("l");
    return data_handler::RELEASE;
  }

  data_handler::result
  usrp2::impl::handle_data_packet(const void *base, size_t len)
  {
    u2_eth_samples_t *pkt = (u2_eth_samples_t *)base;
    d_num_rx_frames++;
    d_num_rx_bytes += len;

    /* --- FIXME start of fake transport layer handler --- */

    if (d_rx_seqno != -1) {
      int expected_seqno = (d_rx_seqno + 1) & 0xFF;
      int seqno = pkt->hdrs.thdr.seqno;

      if (seqno != expected_seqno) {
	::write(2, "S", 1); // missing sequence number
	int missing = seqno - expected_seqno;
	if (missing < 0)
	  missing += 256;

	d_num_rx_overruns++;
	d_num_rx_missing += missing;
      }
    }

    d_rx_seqno = pkt->hdrs.thdr.seqno;

    /* --- end of fake transport layer handler --- */

    // FIXME unaligned load!
    unsigned int chan = u2p_chan(&pkt->hdrs.fixed);

    {
      gruel::scoped_lock l(d_channel_rings_mutex);

      if (!d_channel_rings[chan]) {
	DEBUG_LOG("!");
	return data_handler::RELEASE; 	// discard packet, no channel handler
      }

      // Strip off ethernet header and transport header and enqueue the rest

      size_t offset = offsetof(u2_eth_samples_t, hdrs.fixed);
      if (d_channel_rings[chan]->enqueue(&pkt->hdrs.fixed, len-offset)) {
	inc_enqueued();
	DEBUG_LOG("+");
	return data_handler::KEEP;	// channel ring runner will mark frame done
      }
      else {
	DEBUG_LOG("!");
	return data_handler::RELEASE;	// discard, no room in channel ring
      }
      return data_handler::RELEASE;
    }
  }


  // ----------------------------------------------------------------
  // 			       Receive
  // ----------------------------------------------------------------

  bool
  usrp2::impl::set_rx_antenna(int ant){
    op_config_mimo_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_RX_ANTENNA;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.flags = ant;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    return ntohx(reply.ok) == 1;
  }

  bool
  usrp2::impl::set_rx_gain(double gain)
  {
    op_config_rx_v2_cmd cmd;
    op_config_rx_reply_v2_t reply;

    init_config_rx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_GAIN);
    cmd.op.gain = htons(u2_double_to_fxpt_gain(gain));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::set_rx_lo_offset(double frequency)
  {
    op_freq_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_SET_RX_LO_OFFSET;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;

    u2_fxpt_freq_t fxpt = u2_double_to_fxpt_freq(frequency);
    cmd.op.freq_hi = htonl(u2_fxpt_freq_hi(fxpt));
    cmd.op.freq_lo = htonl(u2_fxpt_freq_lo(fxpt));

    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::set_rx_center_freq(double frequency, tune_result *result)
  {
    op_config_rx_v2_cmd cmd;
    op_config_rx_reply_v2_t reply;

    init_config_rx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_FREQ);
    u2_fxpt_freq_t fxpt = u2_double_to_fxpt_freq(frequency);
    cmd.op.freq_hi = htonl(u2_fxpt_freq_hi(fxpt));
    cmd.op.freq_lo = htonl(u2_fxpt_freq_lo(fxpt));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (result && success) {
      result->baseband_freq =
        u2_fxpt_freq_to_double(
	  u2_fxpt_freq_from_hilo(ntohl(reply.baseband_freq_hi),
				 ntohl(reply.baseband_freq_lo)));

      result->dxc_freq =
        u2_fxpt_freq_to_double(
	  u2_fxpt_freq_from_hilo(ntohl(reply.ddc_freq_hi),
				 ntohl(reply.ddc_freq_lo)));

      result->residual_freq =
        u2_fxpt_freq_to_double(
	 u2_fxpt_freq_from_hilo(ntohl(reply.residual_freq_hi),
				ntohl(reply.residual_freq_lo)));

      result->spectrum_inverted = (bool)(ntohx(reply.inverted) == 1);
    }

    return success;
  }

  bool
  usrp2::impl::set_rx_decim(int decimation_factor)
  {
    op_config_rx_v2_cmd cmd;
    op_config_rx_reply_v2_t reply;

    init_config_rx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_INTERP_DECIM);
    cmd.op.decim = htonl(decimation_factor);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (success)
      d_rx_decim = decimation_factor;
    return success;
  }

  bool
  usrp2::impl::set_rx_scale_iq(int scale_i, int scale_q)
  {
    op_config_rx_v2_cmd cmd;
    op_config_rx_reply_v2_t reply;

    init_config_rx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_SCALE_IQ);
    cmd.op.scale_iq = htonl(((scale_i & 0xffff) << 16) | (scale_q & 0xffff));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::start_rx_streaming(unsigned int channel, unsigned int items_per_frame)
  {
    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel number (" << channel
		<< ")" << std::endl;
      return false;
    }

    if (channel > 0) { // until firmware supports multiple streams
      std::cerr << "usrp2: channel " << channel
		<< " not implemented" << std::endl;
      return false;
    }

    {
      gruel::scoped_lock l(d_channel_rings_mutex);
      if (d_channel_rings[channel]) {
	std::cerr << "usrp2: channel " << channel
		  << " already streaming" << std::endl;
	return false;
      }

      if (items_per_frame == 0)
	items_per_frame = U2_MAX_SAMPLES;		// minimize overhead

      op_start_rx_streaming_cmd cmd;
      op_generic_t reply;

      memset(&cmd, 0, sizeof(cmd));
      init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
      cmd.op.opcode = OP_START_RX_STREAMING;
      cmd.op.len = sizeof(cmd.op);
      cmd.op.rid = d_next_rid++;
      cmd.op.items_per_frame = htonl(items_per_frame);
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);

      d_dont_enqueue = false;
      bool success = false;
      pending_reply p(cmd.op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);

      if (success)
	d_channel_rings[channel] = ring_sptr(new ring(d_eth_buf->max_frames()));
      else
	d_dont_enqueue = true;

      //fprintf(stderr, "usrp2::start_rx_streaming: success = %d\n", success);
      return success;
    }
  }

  bool
  usrp2::impl::start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time)
  {
    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel number (" << channel
		<< ")" << std::endl;
      return false;
    }

    if (channel > 0) { // until firmware supports multiple streams
      std::cerr << "usrp2: channel " << channel
		<< " not implemented" << std::endl;
      return false;
    }

    {
      gruel::scoped_lock guard(d_channel_rings_mutex);
      if (d_channel_rings[channel]) {
	std::cerr << "usrp2: channel " << channel
		  << " already streaming" << std::endl;
	return false;
      }

      if (items_per_frame == 0)
	items_per_frame = U2_MAX_SAMPLES;		// minimize overhead

      op_start_rx_streaming_cmd cmd;
      op_generic_t reply;

      memset(&cmd, 0, sizeof(cmd));
      init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, time);
      cmd.op.opcode = OP_START_RX_STREAMING;
      cmd.op.len = sizeof(cmd.op);
      cmd.op.rid = d_next_rid++;
      cmd.op.items_per_frame = htonl(items_per_frame);
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);

      d_dont_enqueue = false;
      bool success = false;
      pending_reply p(cmd.op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);

      if (success)
	d_channel_rings[channel] = ring_sptr(new ring(d_eth_buf->max_frames()));
      else
	d_dont_enqueue = true;

      return success;
    }
  }

  bool
  usrp2::impl::sync_and_start_rx_streaming_at(unsigned int channel, unsigned int items_per_frame, unsigned int time)
  {

    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel number (" << channel
		<< ")" << std::endl;
      return false;
    }

    if (channel > 0) { // until firmware supports multiple streams
      std::cerr << "usrp2: channel " << channel
		<< " not implemented" << std::endl;
      return false;
    }

    {
      gruel::scoped_lock guard(d_channel_rings_mutex);
      if (d_channel_rings[channel]) {
	std::cerr << "usrp2: channel " << channel
		  << " already streaming" << std::endl;
	return false;
      }

      if (items_per_frame == 0)
	items_per_frame = U2_MAX_SAMPLES;		// minimize overhead

      op_sync_and_start_rx_streaming_cmd cmd;
      op_generic_t reply;

      memset(&cmd, 0, sizeof(cmd));
      init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, time);
      cmd.sync_op.opcode = OP_SYNC_TO_PPS;
      cmd.sync_op.len = sizeof(cmd.sync_op);
      cmd.sync_op.rid = d_next_rid++;
      cmd.rx_op.opcode = OP_START_RX_STREAMING;
      cmd.rx_op.len = sizeof(cmd.rx_op);
      cmd.rx_op.rid = d_next_rid++;
      cmd.rx_op.items_per_frame = htonl(items_per_frame);
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);

      d_dont_enqueue = false;
      bool success = false;
      pending_reply p(cmd.sync_op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);

      if (success)
	d_channel_rings[channel] = ring_sptr(new ring(d_eth_buf->max_frames()));
      else
	d_dont_enqueue = true;

      return success;
    }
  }

  bool
  usrp2::impl::stop_rx_streaming(unsigned int channel)
  {
    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel number (" << channel
		<< ")" << std::endl;
      return false;
    }

    if (channel > 0) { // until firmware supports multiple streams
      std::cerr << "usrp2: channel " << channel
		<< " not implemented" << std::endl;
      return false;
    }

    d_dont_enqueue = true;	// no new samples
    flush_rx_samples(channel);	// dump any we may already have

    op_stop_rx_cmd cmd;
    op_generic_t reply;

    {
      gruel::scoped_lock l(d_channel_rings_mutex);

      memset(&cmd, 0, sizeof(cmd));
      init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
      cmd.op.opcode = OP_STOP_RX;
      cmd.op.len = sizeof(cmd.op);
      cmd.op.rid = d_next_rid++;
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);

      bool success = false;
      pending_reply p(cmd.op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);
      d_channel_rings[channel].reset();
      d_rx_seqno = -1;
      //fprintf(stderr, "usrp2::stop_rx_streaming:  success = %d\n", success);
      return success;
    }
  }

  bool
  usrp2::impl::rx_samples(unsigned int channel, rx_sample_handler *handler)
  {
    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel (" << channel
                << " )" << std::endl;
      return false;
    }

    if (channel > 0) {
      std::cerr << "usrp2: channel " << channel
                << " not implemented" << std::endl;
      return false;
    }

    ring_sptr rp = d_channel_rings[channel];
    if (!rp){
      std::cerr << "usrp2: channel " << channel
                << " not receiving" << std::endl;
      return false;
    }

    // Wait for frames available in channel ring
    DEBUG_LOG("W");
    rp->wait_for_not_empty();
    DEBUG_LOG("s");

    // Iterate through frames and present to user
    void *p;
    size_t frame_len_in_bytes;
    while (rp->dequeue(&p, &frame_len_in_bytes)) {
      uint32_t	       *items;			// points to beginning of data items
      size_t 		nitems_in_uint32s;
      rx_metadata	md;
      if (!parse_rx_metadata(p, frame_len_in_bytes, &items, &nitems_in_uint32s, &md))
	return false;

      bool want_more = (*handler)(items, nitems_in_uint32s, &md);
      d_eth_buf->release_frame(p);
      DEBUG_LOG("-");
      dec_enqueued();

      if (!want_more)
        break;
    }
    return true;
  }

  bool
  usrp2::impl::flush_rx_samples(unsigned int channel)
  {
    if (channel > MAX_CHAN) {
      std::cerr << "usrp2: invalid channel (" << channel
                << " )" << std::endl;
      return false;
    }

    if (channel > 0) {
      std::cerr << "usrp2: channel " << channel
                << " not implemented" << std::endl;
      return false;
    }

    ring_sptr rp = d_channel_rings[channel];
    if (!rp){
      return false;
    }

    // Iterate through frames and drop them
    void *p;
    size_t frame_len_in_bytes;
    while (rp->dequeue(&p, &frame_len_in_bytes)) {
      d_eth_buf->release_frame(p);
      dec_enqueued();
    }
    return true;
  }

  // ----------------------------------------------------------------
  // 				Transmit
  // ----------------------------------------------------------------

  bool
  usrp2::impl::set_tx_antenna(int ant){
    op_config_mimo_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_TX_ANTENNA;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.flags = ant;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    return ntohx(reply.ok) == 1;
  }

  bool
  usrp2::impl::set_tx_gain(double gain)
  {
    op_config_tx_v2_cmd cmd;
    op_config_tx_reply_v2_t reply;

    init_config_tx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_GAIN);
    cmd.op.gain = htons(u2_double_to_fxpt_gain(gain));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::set_tx_lo_offset(double frequency)
  {
    op_freq_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_SET_TX_LO_OFFSET;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;

    u2_fxpt_freq_t fxpt = u2_double_to_fxpt_freq(frequency);
    cmd.op.freq_hi = htonl(u2_fxpt_freq_hi(fxpt));
    cmd.op.freq_lo = htonl(u2_fxpt_freq_lo(fxpt));

    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::set_tx_center_freq(double frequency, tune_result *result)
  {
    op_config_tx_v2_cmd cmd;
    op_config_tx_reply_v2_t reply;

    init_config_tx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_FREQ);
    u2_fxpt_freq_t fxpt = u2_double_to_fxpt_freq(frequency);
    cmd.op.freq_hi = htonl(u2_fxpt_freq_hi(fxpt));
    cmd.op.freq_lo = htonl(u2_fxpt_freq_lo(fxpt));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (result && success) {
      result->baseband_freq =
        u2_fxpt_freq_to_double(
	  u2_fxpt_freq_from_hilo(ntohl(reply.baseband_freq_hi),
				 ntohl(reply.baseband_freq_lo)));

      result->dxc_freq =
        u2_fxpt_freq_to_double(
	  u2_fxpt_freq_from_hilo(ntohl(reply.duc_freq_hi),
				 ntohl(reply.duc_freq_lo)));

      result->residual_freq =
        u2_fxpt_freq_to_double(
	 u2_fxpt_freq_from_hilo(ntohl(reply.residual_freq_hi),
				ntohl(reply.residual_freq_lo)));

      result->spectrum_inverted = (bool)(ntohx(reply.inverted) == 1);
    }

    return success;
  }

  bool
  usrp2::impl::set_tx_interp(int interpolation_factor)
  {
    op_config_tx_v2_cmd cmd;
    op_config_tx_reply_v2_t reply;

    init_config_tx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_INTERP_DECIM);
    cmd.op.interp = htonl(interpolation_factor);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (success) {
      d_tx_interp = interpolation_factor;

      // Auto-set TX scaling based on interpolation rate
      int scale_i, scale_q;
      default_tx_scale_iq(d_tx_interp, &scale_i, &scale_q);
      return set_tx_scale_iq(scale_i, scale_q);
    }

    return success;
  }

  void
  usrp2::impl::default_tx_scale_iq(int interpolation_factor, int *scale_i, int *scale_q)
  {
    // Calculate CIC interpolation (i.e., without halfband interpolators)
    int i = interpolation_factor;
    if (i > 128)
      i = i >> 1;
    if (i > 128)
      i = i >> 1;

    // Calculate dsp_core_tx gain absent scale multipliers
    float gain = (1.65*i*i*i)/(4096*pow(2, ceil(log2(i*i*i))));

    // Calculate closest multiplier constant to reverse gain
    int scale = (int)rint(1.0/gain);
    // fprintf(stderr, "if=%i i=%i gain=%f scale=%i\n", interpolation_factor, i, gain, scale);

    // Both I and Q are identical in this case
    if (scale_i)
      *scale_i = scale;
    if (scale_q)
      *scale_q = scale;
  }

  bool
  usrp2::impl::set_tx_scale_iq(int scale_i, int scale_q)
  {
    op_config_tx_v2_cmd cmd;
    op_config_tx_reply_v2_t reply;

    init_config_tx_v2_cmd(&cmd);
    cmd.op.valid = htons(CFGV_SCALE_IQ);
    cmd.op.scale_iq = htonl(((scale_i & 0xffff) << 16) | (scale_q & 0xffff));

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool
  usrp2::impl::tx_32fc(unsigned int channel,
		       const std::complex<float> *samples,
		       size_t nsamples,
		       const tx_metadata *metadata)
  {
    uint32_t items[nsamples];
    copy_host_32fc_to_u2_16sc(nsamples, samples, items);
    return tx_raw(channel, items, nsamples, metadata);
  }

  bool
  usrp2::impl::tx_16sc(unsigned int channel,
		       const std::complex<int16_t> *samples,
		       size_t nsamples,
		       const tx_metadata *metadata)
  {
#ifdef WORDS_BIGENDIAN

    // Already binary equivalent to 16-bit I/Q on the wire.
    // No conversion required.

    assert(sizeof(samples[0]) == sizeof(uint32_t));
    return tx_raw(channel, (const uint32_t *) samples, nsamples, metadata);

#else

    uint32_t items[nsamples];
    copy_host_16sc_to_u2_16sc(nsamples, samples, items);
    return tx_raw(channel, items, nsamples, metadata);

#endif
  }

  bool
  usrp2::impl::tx_raw(unsigned int channel,
		      const uint32_t *items,
		      size_t nitems,
		      const tx_metadata *metadata)
  {
    if (nitems == 0)
      return true;

    // FIXME can't deal with nitems < U2_MIN_SAMPLES (will be fixed in VRT)
    // FIXME need to check the MTU instead of assuming 1500 bytes

    // fragment as necessary then fire away

    size_t nframes = (nitems + U2_MAX_SAMPLES - 1) / U2_MAX_SAMPLES;
    size_t last_frame = nframes - 1;
    u2_eth_packet_t	hdrs;

    size_t n = 0;
    for (size_t fn = 0; fn < nframes; fn++){
      uint32_t timestamp = 0;
      uint32_t flags = 0;

      if (fn == 0){
	timestamp = metadata->timestamp;
	if (metadata->send_now)
	  flags |= U2P_TX_IMMEDIATE;
	if (metadata->start_of_burst)
	  flags |= U2P_TX_START_OF_BURST;
      }
      if (fn > 0){
	flags |= U2P_TX_IMMEDIATE;
      }
      if (fn == last_frame){
	if (metadata->end_of_burst)
	  flags |= U2P_TX_END_OF_BURST;
      }

      init_etf_hdrs(&hdrs, d_addr, flags, channel, timestamp);

      // Avoid short packet by splitting last two packets if reqd
      size_t i;
      if ((nitems - n) > U2_MAX_SAMPLES && (nitems - n) < (U2_MAX_SAMPLES + U2_MIN_SAMPLES))
	i = (nitems - n) / 2;
      else
	i = std::min((size_t) U2_MAX_SAMPLES, nitems - n);

      eth_iovec iov[2];
      iov[0].iov_base = &hdrs;
      iov[0].iov_len = sizeof(hdrs);
      iov[1].iov_base = const_cast<uint32_t *>(&items[n]);
      iov[1].iov_len = i * sizeof(uint32_t);

      size_t total = iov[0].iov_len + iov[1].iov_len;
      if (total < 64)
	fprintf(stderr, "usrp2::tx_raw: FIXME: short packet: %zd items (%zd bytes)\n", i, total);

      if (d_eth_buf->tx_framev(iov, 2) != eth_buffer::EB_OK){
	return false;
      }

      n += i;
    }

    return true;
  }

  // ----------------------------------------------------------------
  // 			   misc commands
  // ----------------------------------------------------------------

  bool
  usrp2::impl::config_mimo(int flags)
  {
    op_config_mimo_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_CONFIG_MIMO;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.flags = flags;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    return ntohx(reply.ok) == 1;
  }

  bool
  usrp2::impl::fpga_master_clock_freq(long *freq)
  {
    *freq = 100000000L;		// 100 MHz
    return true;
  }

  bool
  usrp2::impl::adc_rate(long *rate)
  {
    return fpga_master_clock_freq(rate);
  }

  bool
  usrp2::impl::dac_rate(long *rate)
  {
    return fpga_master_clock_freq(rate);
  }

  bool
  usrp2::impl::tx_daughterboard_id(int *dbid)
  {
    *dbid = d_tx_db_info.dbid;
    return true;
  }

  bool
  usrp2::impl::rx_daughterboard_id(int *dbid)
  {
    *dbid = d_rx_db_info.dbid;
    return true;
  }


  // ----------------------------------------------------------------
  // 			low-level commands
  // ----------------------------------------------------------------

  bool
  usrp2::impl::burn_mac_addr(const std::string &new_addr)
  {
    op_burn_mac_addr_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_BURN_MAC_ADDR;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    if (!parse_mac_addr(new_addr, &cmd.op.addr))
      return false;

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, 4*DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  static void
  fill_dboard_info(db_info *dst, const u2_db_info_t *src)
  {
    dst->dbid = ntohl(src->dbid);

    dst->freq_min =
      u2_fxpt_freq_to_double(u2_fxpt_freq_from_hilo(ntohl(src->freq_min_hi),
						    ntohl(src->freq_min_lo)));
    dst->freq_max =
      u2_fxpt_freq_to_double(u2_fxpt_freq_from_hilo(ntohl(src->freq_max_hi),
						    ntohl(src->freq_max_lo)));

    dst->gain_min = u2_fxpt_gain_to_double(ntohs(src->gain_min));
    dst->gain_max = u2_fxpt_gain_to_double(ntohs(src->gain_max));
    dst->gain_step_size = u2_fxpt_gain_to_double(ntohs(src->gain_step_size));
  }

  bool
  usrp2::impl::dboard_info()
  {
    op_dboard_info_cmd  	cmd;
    op_dboard_info_reply_t	reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_DBOARD_INFO;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (success){
      fill_dboard_info(&d_tx_db_info, &reply.tx_db_info);
      fill_dboard_info(&d_rx_db_info, &reply.rx_db_info);
    }
    return success;
  }


  bool
  usrp2::impl::sync_to_pps()
  {
    op_generic_cmd cmd;
    op_generic_t   reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_SYNC_TO_PPS;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    return ntohx(reply.ok) == 1;
  }

  bool
  usrp2::impl::sync_every_pps(bool enable)
  {
    op_generic_cmd cmd;
    op_generic_t   reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_SYNC_EVERY_PPS;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.ok = enable ? 1 : 0;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    return ntohx(reply.ok) == 1;
  }

  std::vector<uint32_t>
  usrp2::impl::peek32(uint32_t addr, uint32_t words)
  {
    std::vector<uint32_t> result; // zero sized on error return
    // fprintf(stderr, "usrp2::peek: addr=%08X words=%u\n", addr, words);

    if (addr % 4 != 0) {
      fprintf(stderr, "usrp2::peek: addr (=%08X) must be 32-bit word aligned\n", addr);
      return result;
    }

    if (words == 0)
      return result;

    op_peek_cmd   cmd;
    op_generic_t *reply;

    int wlen = sizeof(uint32_t);
    int rlen = sizeof(op_generic_t);
    size_t bytes = words*wlen;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_PEEK;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    cmd.op.addr = htonl(addr);
    cmd.op.bytes = htonl(bytes);

    reply = (op_generic_t *)malloc(rlen+bytes);
    pending_reply p(cmd.op.rid, reply, rlen+bytes);
    if (transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT)) {
      uint32_t nwords = (reply->len-rlen)/sizeof(uint32_t);
      uint32_t *data = (uint32_t *)(reply+rlen/wlen);
      for (unsigned int i = 0; i < nwords; i++)
	result.push_back(ntohl(data[i]));
    }

    free(reply);
    return result;
  }

  bool
  usrp2::impl::poke32(uint32_t addr, const std::vector<uint32_t> &data)
  {
    if (addr % 4 != 0) {
      fprintf(stderr, "usrp2::poke32: addr (=%08X) must be 32-bit word aligned\n", addr);
      return false;
    }

    int plen = sizeof(op_poke_cmd);
    int wlen = sizeof(uint32_t);
    int max_words = (MAX_SUBPKT_LEN-plen)/wlen;
    int words = data.size();

    if (words > max_words) {
      fprintf(stderr, "usrp2::poke32: write size (=%u) exceeds maximum of %u words\n",
	      words, max_words);
      return false;
    }

    //fprintf(stderr, "usrp2::poke32: addr=%08X words=%u\n", addr, words);

    if (words == 0)
      return true; // NOP

    op_poke_cmd  *cmd;
    op_generic_t *eop;

    // Allocate, clear, and initialize command packet
    int bytes = words*wlen;
    int l = plen+bytes+sizeof(*eop); // op_poke_cmd+data+eop
    cmd = (op_poke_cmd *)malloc(l);
    //fprintf(stderr, "cmd=%p l=%i\n", cmd, l);
    memset(cmd, 0, l);
    init_etf_hdrs(&cmd->h, d_addr, 0, CONTROL_CHAN, -1);
    cmd->op.opcode = OP_POKE;
    cmd->op.len = sizeof(cmd->op)+bytes;
    cmd->op.rid = d_next_rid++;
    cmd->op.addr = htonl(addr);

    // Copy data from vector into packet space
    uint32_t *dest = (uint32_t *)((uint8_t *)cmd+plen);
    for (int i = 0; i < words; i++) {
      //fprintf(stderr, "%03i@%p\n", i, dest);
      *dest++ = htonl(data[i]);
    }

    // Write end-of-packet subpacket
    eop = (op_generic_t *)dest;
    eop->opcode = OP_EOP;
    eop->len = sizeof(*eop);
    //fprintf(stderr, "eop=%p len=%i\n", eop, eop->len);

    // Send command to device and retrieve reply
    bool ok = false;
    op_generic_t reply;
    pending_reply p(cmd->op.rid, &reply, sizeof(reply));
    if (transmit_cmd_and_wait(cmd, l, &p, DEF_CMD_TIMEOUT))
      ok = (ntohx(reply.ok) == 1);

    free(cmd);
    return ok;
  }

  bool
  usrp2::impl::reset_db()
  {
    op_generic_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_RESET_DB;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool usrp2::impl::set_gpio_ddr(int bank, uint16_t value, uint16_t mask)
  {
    if (bank != GPIO_TX_BANK && bank != GPIO_RX_BANK) {
      fprintf(stderr, "set_gpio_ddr: bank must be one of GPIO_RX_BANK or GPIO_TX_BANK\n");
      return false;
    }

    op_gpio_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_GPIO_SET_DDR;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.bank = static_cast<uint8_t>(bank);
    cmd.op.value = htons(value);
    cmd.op.mask = htons(mask);
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool usrp2::impl::set_gpio_sels(int bank, std::string sels)
  {
    if (bank != GPIO_TX_BANK && bank != GPIO_RX_BANK) {
      fprintf(stderr, "set_gpio_ddr: bank must be one of GPIO_RX_BANK or GPIO_TX_BANK\n");
      return false;
    }

    if (sels.size() != 16) {
      fprintf(stderr, "set_gpio_sels: sels must be exactly 16 bytes\n");
      return false;
    }

    op_gpio_set_sels_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_GPIO_SET_SELS;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.bank = static_cast<uint8_t>(bank);
    memcpy(&cmd.op.sels, sels.c_str(), 16);
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool usrp2::impl::write_gpio(int bank, uint16_t value, uint16_t mask)
  {
    if (bank != GPIO_TX_BANK && bank != GPIO_RX_BANK) {
      fprintf(stderr, "set_gpio_ddr: bank must be one of GPIO_RX_BANK or GPIO_TX_BANK\n");
      return false;
    }

    op_gpio_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_GPIO_WRITE;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.bank = static_cast<uint8_t>(bank);
    cmd.op.value = htons(value);
    cmd.op.mask = htons(mask);
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

  bool usrp2::impl::read_gpio(int bank, uint16_t *value)
  {
    if (bank != GPIO_TX_BANK && bank != GPIO_RX_BANK) {
      fprintf(stderr, "set_gpio_ddr: bank must be one of GPIO_RX_BANK or GPIO_TX_BANK\n");
      return false;
    }

    op_gpio_cmd cmd;
    op_gpio_read_reply_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_GPIO_READ;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.bank = static_cast<uint8_t>(bank);
    cmd.op.value = 0; // not used
    cmd.op.mask = 0;  // not used
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    if (success && (value != NULL))
      *value = ntohs(reply.value);

    return success;
  }

  bool usrp2::impl::enable_gpio_streaming(int bank, int enable)
  {
    if (bank != GPIO_RX_BANK) {
      fprintf(stderr, "enable_gpio_streaming: only RX streaming is currently implemented\n");
      return false;
    }

    if ((enable & ~0x03) != 0) {
      fprintf(stderr, "enable_gpio_streaming: invalid enable format\n");
      return false;
    }

    op_gpio_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    init_etf_hdrs(&cmd.h, d_addr, 0, CONTROL_CHAN, -1);
    cmd.op.opcode = OP_GPIO_STREAM;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    cmd.op.bank = static_cast<uint8_t>(bank);
    cmd.op.value = htons((uint16_t)enable);
    cmd.op.mask = 0;  // not used
    cmd.eop.opcode = OP_EOP;
    cmd.eop.len = sizeof(cmd.eop);

    pending_reply p(cmd.op.rid, &reply, sizeof(reply));
    if (!transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT))
      return false;

    bool success = (ntohx(reply.ok) == 1);
    return success;
  }

} // namespace usrp2
