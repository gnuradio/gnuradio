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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include <usrp2/tune_result.h>
#include <usrp2/copiers.h>
#include <gruel/inet.h>
#include <gruel/realtime.h>
#include <gruel/sys_pri.h>
#include <usrp2_types.h>
#include "usrp2_impl.h"
#include "control.h"
#include <stdexcept>
#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

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

  /*********************************************************************
   * control packet handler for control packets
   *   creates a data handler with access to the pending replies
   *   the operator() is called by the transport for each packet
   ********************************************************************/
  class ctrl_packet_handler : public data_handler{
  private:
    pending_reply **d_pending_replies;

  public:
    ctrl_packet_handler(pending_reply **pending_replies): d_pending_replies(pending_replies){}

    data_handler::result operator()(const void *base, size_t len){
        // point to beginning of payload (subpackets)
        unsigned char *p = (unsigned char *)base;

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
          d_pending_replies[rid] = NULL;
          return data_handler::DONE;
        }

        // TODO: handle unsolicited, USRP2 initiated, or late replies
        DEBUG_LOG("l");
        return data_handler::DONE;
    }
  };

  /*********************************************************************
   * data packet handler for incoming samples
   *   creates a data handler with rx sample handler
   *   the operator() is called by the transport for each packet
   ********************************************************************/
  class data_packet_handler : public data_handler{
  private:
    vrt::rx_packet_handler *d_handler;

  public:
    data_packet_handler(vrt::rx_packet_handler *handler): d_handler(handler){}

    data_handler::result operator()(const void *base, size_t len){
        DEBUG_LOG("h"); 
        vrt::expanded_header hdr;
        const uint32_t *payload;
        size_t n32_bit_words_payload;
        //parse the vrt header and store into the ring data structure
        if (not vrt::expanded_header::parse(
            (const uint32_t*)base,len/sizeof(uint32_t), //in
            &hdr, &payload, &n32_bit_words_payload) //out
            or not hdr.stream_id_p()
        ){
            printf("Bad vrt header 0x%.8x, Packet len %d\n", hdr.header, (int)len);
            DEBUG_LOG("!");
            return data_handler::RELEASE;
        }
        bool want_more = (*d_handler)(payload, n32_bit_words_payload, &hdr);

        return want_more? data_handler::RELEASE : data_handler::DONE;
    }
  };

  usrp2::impl::impl(transport::sptr data_transport, transport::sptr ctrl_transport) :
      d_next_rid(0),
      d_tx_pkt_cnt(0),
      d_tx_interp(0),
      d_rx_decim(0),
      d_ctrl_transport(ctrl_transport),
      d_data_transport(data_transport)
  {
    d_ctrl_thread_running = true;
    d_ctrl_thread = new boost::thread(boost::bind(&usrp2::impl::ctrl_thread_loop, this));
    memset(d_pending_replies, 0, sizeof(d_pending_replies));

    // In case the USRP2 was left streaming RX
    stop_rx_streaming();

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
    //stop the control thread
    d_ctrl_thread_running = false;
    d_ctrl_thread->interrupt();
    d_ctrl_thread->join();

  }

  void
  usrp2::impl::init_config_rx_v2_cmd(op_config_rx_v2_cmd *cmd)
  {
    memset(cmd, 0, sizeof(*cmd));
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
    cmd->op.opcode = OP_CONFIG_TX_V2;
    cmd->op.len = sizeof(cmd->op);
    cmd->op.rid = d_next_rid++;
    cmd->eop.opcode = OP_EOP;
    cmd->eop.len = sizeof(cmd->eop);
  }


  bool
  usrp2::impl::transmit_cmd(void *cmd, size_t len)
  {
    iovec iov;
    iov.iov_base = cmd;
    iov.iov_len = len;
    return d_ctrl_transport->sendv(&iov, 1);
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

  void 
  usrp2::impl::ctrl_thread_loop(void)
  {
    data_handler *handler = new ctrl_packet_handler(d_pending_replies);
    while (d_ctrl_thread_running){
        d_ctrl_transport->recv(handler);
    }
    delete handler;
  }

  // ----------------------------------------------------------------
  // 			       Receive
  // ----------------------------------------------------------------

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
  usrp2::impl::start_rx_streaming(unsigned int items_per_frame, const time_spec_t *time_spec)
  {

    //flush any old samples in the data transport
    d_data_transport->flush();

      if (items_per_frame == 0)
        items_per_frame = U2_MAX_SAMPLES;		// minimize overhead

      op_start_rx_streaming_cmd cmd;
      op_generic_t reply;

      memset(&cmd, 0, sizeof(cmd));
      cmd.op.opcode = OP_START_RX_STREAMING;
      cmd.op.len = sizeof(cmd.op);
      cmd.op.rid = d_next_rid++;
      cmd.op.items_per_frame = htonl(items_per_frame);
      cmd.op.time_secs = time_spec->secs;
      cmd.op.time_tics = time_spec->tics;
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);

      bool success = false;
      pending_reply p(cmd.op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);

      //fprintf(stderr, "usrp2::start_rx_streaming: success = %d\n", success);
      return success;

  }
  
  bool
  usrp2::impl::stop_rx_streaming()
  {

    op_stop_rx_cmd cmd;
    op_generic_t reply;

    {

      memset(&cmd, 0, sizeof(cmd));
      cmd.op.opcode = OP_STOP_RX;
      cmd.op.len = sizeof(cmd.op);
      cmd.op.rid = d_next_rid++;
      cmd.eop.opcode = OP_EOP;
      cmd.eop.len = sizeof(cmd.eop);
    
      bool success = false;
      pending_reply p(cmd.op.rid, &reply, sizeof(reply));
      success = transmit_cmd_and_wait(&cmd, sizeof(cmd), &p, DEF_CMD_TIMEOUT);
      success = success && (ntohx(reply.ok) == 1);
      //fprintf(stderr, "usrp2::stop_rx_streaming:  success = %d\n", success);
      return success;
    }
  }

  bool
  usrp2::impl::rx_samples(vrt::rx_packet_handler *handler)
  {
    data_handler *pkt_handler = new data_packet_handler(handler);
    d_data_transport->recv(pkt_handler);
    delete pkt_handler;
    return true;
  }

  // ----------------------------------------------------------------
  // 				Transmit
  // ----------------------------------------------------------------

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
  usrp2::impl::tx_32fc(
		       const std::complex<float> *samples,
		       size_t nsamples,
		       const vrt::expanded_header *hdr)
  {
    uint32_t items[nsamples];
    copy_host_32fc_to_u2_16sc(nsamples, samples, items);
    return tx_raw(items, nsamples, hdr);
  }

  bool
  usrp2::impl::tx_16sc(
		       const std::complex<int16_t> *samples,
		       size_t nsamples,
		       const vrt::expanded_header *hdr)
  {
#ifdef WORDS_BIGENDIAN

    // Already binary equivalent to 16-bit I/Q on the wire.
    // No conversion required.

    assert(sizeof(samples[0]) == sizeof(uint32_t));
    return tx_raw((const uint32_t *) samples, nsamples, metadata);

#else

    uint32_t items[nsamples];
    copy_host_16sc_to_u2_16sc(nsamples, samples, items);
    return tx_raw(items, nsamples, hdr);

#endif
  }

  bool
  usrp2::impl::tx_raw(
		      const uint32_t *items,
		      size_t nitems,
		      const vrt::expanded_header *hdr)
  {
    if (nitems == 0)
      return true;

    // FIXME need to check the transport's max size before fragmenting

    // fragment as necessary then fire away

    size_t nframes = (nitems + U2_MAX_SAMPLES - 1) / U2_MAX_SAMPLES;

    uint32_t header[vrt::HEADER_MAX_N32_BIT_WORDS];
    size_t n32_bit_words_header;
    uint32_t trailer[vrt::TRAILER_MAX_N32_BIT_WORDS];
    size_t n32_bit_words_trailer;
    vrt::expanded_header hdr_frag;
    memcpy(&hdr_frag, hdr, sizeof(vrt::expanded_header));

    size_t items_sent = 0;
    for (size_t fn = 0; fn < nframes; fn++){

      //calculate the payload length
      size_t n32_bit_words_payload = std::min((size_t) U2_MAX_SAMPLES, nitems - items_sent);

      //clear the burst flags and count
      hdr_frag.header &= ~(VRTH_START_OF_BURST | VRTH_END_OF_BURST | VRTH_PKT_CNT_MASK);
      //set the packet count //FIXME packet count should be per stream id
      hdr_frag.header |= ((d_tx_pkt_cnt++ << VRTH_PKT_CNT_SHIFT) & VRTH_PKT_CNT_MASK);
      //start of burst can only be set on the first fragment
      if (hdr->header & VRTH_START_OF_BURST and fn == 0)
        hdr_frag.header |= VRTH_START_OF_BURST;
      //end of burst can only be set on the last fragment
      if (hdr->header & VRTH_END_OF_BURST and fn == nframes - 1)
        hdr_frag.header |= VRTH_END_OF_BURST;

      //--------- load the header, trailer, and word counts ----------//
      vrt::expanded_header::unparse(&hdr_frag, n32_bit_words_payload,    // in
        header, &n32_bit_words_header, trailer, &n32_bit_words_trailer); // out

      //------- pack the iovecs with the header, data, trailer -------//
      iovec iov[3];
      iov[0].iov_base = header;
      iov[0].iov_len  = n32_bit_words_header*sizeof(uint32_t);
      iov[1].iov_base = const_cast<uint32_t *>(&items[items_sent]);
      iov[1].iov_len  = n32_bit_words_payload*sizeof(uint32_t);
      iov[2].iov_base = trailer;
      iov[2].iov_len  = n32_bit_words_trailer*sizeof(uint32_t);

      if (not d_data_transport->sendv(iov, dimof(iov))){
        return false;
      }

      items_sent += n32_bit_words_payload;
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
  usrp2::impl::burn_mac_addr(u2_mac_addr_t *new_mac)
  {
    op_burn_mac_addr_cmd cmd;
    op_generic_t reply;

    memset(&cmd, 0, sizeof(cmd));
    cmd.op.opcode = OP_BURN_MAC_ADDR;
    cmd.op.len = sizeof(cmd.op);
    cmd.op.rid = d_next_rid++;
    memcpy(&cmd.op.addr, new_mac, sizeof(u2_mac_addr_t));

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
