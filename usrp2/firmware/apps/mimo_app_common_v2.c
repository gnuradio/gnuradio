/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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
#include "config.h"
#endif

#include "mimo_app_common_v2.h"
#include "buffer_pool.h"
#include "memcpy_wa.h"
#include "ethernet.h"
#include "nonstdio.h"
#include "print_rmon_regs.h"
#include "db.h"
#include "db_base.h"
#include "clocks.h"
#include "u2_init.h"
#include <string.h>

volatile bool link_is_up = false;	// eth handler sets this
int cpu_tx_buf_dest_port = PORT_ETH;

// If this is non-zero, this dbsm could be writing to the ethernet
dbsm_t *ac_could_be_sending_to_eth;

static unsigned char exp_seqno __attribute__((unused)) = 0;

void abort(void);

static bool
burn_mac_addr(const op_burn_mac_addr_t *p)
{
  return ethernet_set_mac_addr(&p->addr);
}

static bool
sync_to_pps(const op_generic_t *p)
{
  timesync_regs->sync_on_next_pps = 1;
  putstr("SYNC to PPS\n");
  return true;
}

static bool
config_mimo_cmd(const op_config_mimo_t *p)
{
  clocks_mimo_config(p->flags);
  return true;
}

void
set_reply_hdr(u2_eth_packet_t *reply_pkt, u2_eth_packet_t const *cmd_pkt)
{
  reply_pkt->ehdr.dst = cmd_pkt->ehdr.src;
  reply_pkt->ehdr.src = *ethernet_mac_addr();
  reply_pkt->ehdr.ethertype = U2_ETHERTYPE;
  reply_pkt->thdr.flags = 0;
  reply_pkt->thdr.fifo_status = 0;	// written by protocol engine
  reply_pkt->thdr.seqno = 0;		// written by protocol engine
  reply_pkt->thdr.ack = 0;		// written by protocol engine
  u2p_set_word0(&reply_pkt->fixed, 0, CONTROL_CHAN);
  reply_pkt->fixed.timestamp = timer_regs->time;
}

static void
send_reply(unsigned char *reply, size_t reply_len)
{
  if (reply_len < 64)
    reply_len = 64;

  // wait for buffer to become idle
  hal_set_leds(0x4, 0x4);
  while((buffer_pool_status->status & BPS_IDLE(CPU_TX_BUF)) == 0)
    ;
  hal_set_leds(0x0, 0x4);

  // copy reply into CPU_TX_BUF
  memcpy_wa(buffer_ram(CPU_TX_BUF), reply, reply_len);

  // wait until nobody else is sending to the ethernet
  if (ac_could_be_sending_to_eth){
    hal_set_leds(0x8, 0x8);
    dbsm_wait_for_opening(ac_could_be_sending_to_eth);
    hal_set_leds(0x0, 0x8);
  }

  if (0){
    printf("sending_reply to port %d, len = %d\n", cpu_tx_buf_dest_port, (int)reply_len);
    print_buffer(buffer_ram(CPU_TX_BUF), reply_len/4);
  }

  // fire it off
  bp_send_from_buf(CPU_TX_BUF, cpu_tx_buf_dest_port, 1, 0, reply_len/4);

  // wait for it to complete (not long, it's a small pkt)
  while((buffer_pool_status->status & (BPS_DONE(CPU_TX_BUF) | BPS_ERROR(CPU_TX_BUF))) == 0)
    ;

  bp_clear_buf(CPU_TX_BUF);
}


static size_t
op_id_cmd(const op_generic_t *p,
	  void *reply_payload, size_t reply_payload_space)
{
  op_id_reply_t *r = (op_id_reply_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))	// no room
    return 0;

  // Build reply subpacket

  r->opcode = OP_ID_REPLY;
  r->len = sizeof(op_id_reply_t);
  r->rid = p->rid;
  r->addr = *ethernet_mac_addr();
  r->hw_rev = (u2_hw_rev_major << 8) | u2_hw_rev_minor;
  // r->fpga_md5sum = ;	// FIXME
  // r->sw_md5sum = ;	// FIXME

  return r->len;
}


static size_t
config_tx_v2_cmd(const op_config_tx_v2_t *p,
		 void *reply_payload, size_t reply_payload_space)
{
  op_config_tx_reply_v2_t *r = (op_config_tx_reply_v2_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))
    return 0;					// no room

  struct tune_result	tune_result;
  memset(&tune_result, 0, sizeof(tune_result));

  bool ok = true;
  
#if 0
  if (p->valid & CFGV_GAIN){
    ok &= db_set_gain(tx_dboard, p->gain);
  }

  if (p->valid & CFGV_FREQ){
    bool was_streaming = is_streaming();
    if (was_streaming)
      stop_rx_cmd();
    
    u2_fxpt_freq_t f = u2_fxpt_freq_from_hilo(p->freq_hi, p->freq_lo);
    bool tune_ok = db_tune(tx_dboard, f, &tune_result);
    ok &= tune_ok;
    print_tune_result("Tx", tune_ok, f, &tune_result);

    if (was_streaming)
      restart_streaming();
  }

  if (p->valid & CFGV_INTERP_DECIM){
    int interp = p->interp;
    int hb1 = 0;
    int hb2 = 0;

    if (!(interp & 1)){
      hb2 = 1;
      interp = interp >> 1;
    }

    if (!(interp & 1)){
      hb1 = 1;
      interp = interp >> 1;
    }
    
    if (interp < MIN_CIC_INTERP || interp > MAX_CIC_INTERP)
      ok = false;
    else {
      dsp_tx_regs->interp_rate = (hb1<<9) | (hb2<<8) | interp;
      // printf("Interp: %d, register %d\n", p->interp, (hb1<<9) | (hb2<<8) | interp);
    }
  }

  if (p->valid & CFGV_SCALE_IQ){
    dsp_tx_regs->scale_iq = p->scale_iq;
  }
#endif

  // Build reply subpacket

  r->opcode = OP_CONFIG_TX_REPLY_V2;
  r->len = sizeof(*r);
  r->rid = p->rid;
  r->ok = ok;
  r->inverted = tune_result.inverted;
  r->baseband_freq_hi = u2_fxpt_freq_hi(tune_result.baseband_freq);
  r->baseband_freq_lo = u2_fxpt_freq_lo(tune_result.baseband_freq);
  r->duc_freq_hi = u2_fxpt_freq_hi(tune_result.dxc_freq);
  r->duc_freq_lo = u2_fxpt_freq_lo(tune_result.dxc_freq);
  r->residual_freq_hi = u2_fxpt_freq_hi(tune_result.residual_freq);
  r->residual_freq_lo = u2_fxpt_freq_lo(tune_result.residual_freq);
  return r->len;
}

static size_t
config_rx_v2_cmd(const op_config_rx_v2_t *p, 
		 void *reply_payload, size_t reply_payload_space)
{
  op_config_rx_reply_v2_t *r = (op_config_rx_reply_v2_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))
    return 0;				// no room

  struct tune_result	tune_result;
  memset(&tune_result, 0, sizeof(tune_result));

  bool ok = true;
  
  if (p->valid & CFGV_GAIN){
    ok &= db_set_gain(rx_dboard, p->gain);
  }

  if (p->valid & CFGV_FREQ){
    bool was_streaming = is_streaming();
    if (was_streaming)
      stop_rx_cmd();
    
    u2_fxpt_freq_t f = u2_fxpt_freq_from_hilo(p->freq_hi, p->freq_lo);
    bool tune_ok = db_tune(rx_dboard, f, &tune_result);
    ok &= tune_ok;
    print_tune_result("Rx", tune_ok, f, &tune_result);

    if (was_streaming)
      restart_streaming();
  }

  if (p->valid & CFGV_INTERP_DECIM){
    int decim = p->decim;
    int hb1 = 0;
    int hb2 = 0;
    
    if(!(decim & 1)) {
      hb2 = 1;
      decim = decim >> 1;
    }
    
    if(!(decim & 1)) {
      hb1 = 1;
      decim = decim >> 1;
    }
    
    if (decim < MIN_CIC_DECIM || decim > MAX_CIC_DECIM)
      ok = false;
    else {
      dsp_rx_regs->decim_rate = (hb1<<9) | (hb2<<8) | decim;
      // printf("Decim: %d, register %d\n", p->decim, (hb1<<9) | (hb2<<8) | decim);
    }
  }

  if (p->valid & CFGV_SCALE_IQ){
    dsp_rx_regs->scale_iq = p->scale_iq;
  }

  // Build reply subpacket

  r->opcode = OP_CONFIG_RX_REPLY_V2;
  r->len = sizeof(*r);
  r->rid = p->rid;
  r->ok = ok;
  r->inverted = tune_result.inverted;
  r->baseband_freq_hi = u2_fxpt_freq_hi(tune_result.baseband_freq);
  r->baseband_freq_lo = u2_fxpt_freq_lo(tune_result.baseband_freq);
  r->ddc_freq_hi = u2_fxpt_freq_hi(tune_result.dxc_freq);
  r->ddc_freq_lo = u2_fxpt_freq_lo(tune_result.dxc_freq);
  r->residual_freq_hi = u2_fxpt_freq_hi(tune_result.residual_freq);
  r->residual_freq_lo = u2_fxpt_freq_lo(tune_result.residual_freq);

  return r->len;
}

static size_t
read_time_cmd(const op_generic_t *p,
	      void *reply_payload, size_t reply_payload_space)
{
  op_read_time_reply_t *r = (op_read_time_reply_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))		
    return 0;					// no room

  r->opcode = OP_READ_TIME_REPLY;
  r->len = sizeof(*r);
  r->rid = p->rid;
  r->time = timer_regs->time;

  return r->len;
}

static void
fill_db_info(u2_db_info_t *p, const struct db_base *db)
{
  p->dbid = db->dbid;
  p->freq_min_hi = u2_fxpt_freq_hi(db->freq_min);
  p->freq_min_lo = u2_fxpt_freq_lo(db->freq_min);
  p->freq_max_hi = u2_fxpt_freq_hi(db->freq_max);
  p->freq_max_lo = u2_fxpt_freq_lo(db->freq_max);
  p->gain_min = db->gain_min;
  p->gain_max = db->gain_max;
  p->gain_step_size = db->gain_step_size;
}

static size_t
dboard_info_cmd(const op_generic_t *p,
		void *reply_payload, size_t reply_payload_space)
{
  op_dboard_info_reply_t *r = (op_dboard_info_reply_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))		
    return 0;					// no room

  r->opcode = OP_DBOARD_INFO_REPLY;
  r->len = sizeof(*r);
  r->rid = p->rid;
  r->ok = true;

  fill_db_info(&r->tx_db_info, tx_dboard);
  fill_db_info(&r->rx_db_info, rx_dboard);

  return r->len;
}

static size_t
peek_cmd(const op_peek_t *p,
	 void *reply_payload, size_t reply_payload_space)
{
  op_generic_t *r = (op_generic_t *) reply_payload;

  putstr("peek: addr="); puthex32(p->addr);
  printf(" bytes=%u\n", p->bytes);

  if ((reply_payload_space < (sizeof(*r) + p->bytes)) ||
      p->bytes > MAX_SUBPKT_LEN - sizeof(op_generic_t)) {
    putstr("peek: insufficient reply packet space\n");
    return 0;			// FIXME do partial read?
  }

  r->opcode = OP_PEEK_REPLY;
  r->len = sizeof(*r)+p->bytes;
  r->rid = p->rid;
  r->ok = true;

  memcpy_wa(reply_payload+sizeof(*r), (void *)p->addr, p->bytes);

  return r->len;
}

static bool
poke_cmd(const op_poke_t *p)
{
  int bytes = p->len - sizeof(*p);
  putstr("poke: addr="); puthex32(p->addr);
  printf(" bytes=%u\n", bytes);

  uint8_t *src = (uint8_t *)p + sizeof(*p);
  memcpy_wa((void *)p->addr, src, bytes);

  return true;
}

static size_t
generic_reply(const op_generic_t *p,
	      void *reply_payload, size_t reply_payload_space,
	      bool ok)
{
  op_generic_t *r = (op_generic_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))		
    return 0;					// no room

  r->opcode = p->opcode | OP_REPLY_BIT;
  r->len = sizeof(*r);
  r->rid = p->rid;
  r->ok = ok;

  return r->len;
}

static size_t
add_eop(void *reply_payload, size_t reply_payload_space)
{
  op_generic_t *r = (op_generic_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))		
    return 0;					// no room

  r->opcode = OP_EOP;
  r->len = sizeof(*r);
  r->rid = 0;
  r->ok =  0;

  return r->len;
}

void
handle_control_chan_frame(u2_eth_packet_t *pkt, size_t len)
{
  unsigned char reply[sizeof(u2_eth_packet_t) + 4 * sizeof(u2_subpkt_t)] _AL4;
  unsigned char *reply_payload = &reply[sizeof(u2_eth_packet_t)];
  int reply_payload_space = sizeof(reply) - sizeof(u2_eth_packet_t);

  // initialize reply
  memset(reply, 0, sizeof(reply));
  set_reply_hdr((u2_eth_packet_t *) reply, pkt);

  // point to beginning of payload (subpackets)
  unsigned char *payload = ((unsigned char *) pkt) + sizeof(u2_eth_packet_t);
  int payload_len = len - sizeof(u2_eth_packet_t);

  size_t subpktlen = 0;

  while (payload_len >= sizeof(op_generic_t)){
    const op_generic_t *gp = (const op_generic_t *) payload;
    subpktlen = 0;

    // printf("\nopcode = %d\n", gp->opcode);

    switch(gp->opcode){
    case OP_EOP:		// end of subpackets
      goto end_of_subpackets;

    case OP_ID:
      subpktlen = op_id_cmd(gp, reply_payload, reply_payload_space);
      break;
    
    case OP_CONFIG_TX_V2:
      subpktlen = config_tx_v2_cmd((op_config_tx_v2_t *) payload,
				   reply_payload, reply_payload_space);
      break;

    case OP_CONFIG_RX_V2:
      subpktlen = config_rx_v2_cmd((op_config_rx_v2_t *) payload,
				   reply_payload, reply_payload_space);
      break;

    case OP_START_RX_STREAMING:
      start_rx_streaming_cmd(&pkt->ehdr.src, (op_start_rx_streaming_t *) payload);
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space, true);
      break;
    
    case OP_STOP_RX:
      stop_rx_cmd();
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space, true);
      break;
    
    case OP_BURN_MAC_ADDR:
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space,
				burn_mac_addr((op_burn_mac_addr_t *) payload));
      break;

    case OP_CONFIG_MIMO:
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space,
				config_mimo_cmd((op_config_mimo_t *) payload));
      break;

    case OP_READ_TIME:
      subpktlen = read_time_cmd(gp, reply_payload, reply_payload_space);
      break;

    case OP_DBOARD_INFO:
      subpktlen = dboard_info_cmd(gp, reply_payload, reply_payload_space);
      break;

    case OP_SYNC_TO_PPS:
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space,
				sync_to_pps((op_generic_t *) payload));
      break;

    case OP_PEEK:
      subpktlen = peek_cmd((op_peek_t *)payload, reply_payload, reply_payload_space);
      break;

    case OP_POKE:
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space,
				poke_cmd((op_poke_t *)payload));
      break;

    default:
      printf("app_common_v2: unhandled opcode = %d\n", gp->opcode);
      break;
    }

    int t = (gp->len + 3) & ~3;		// bump to a multiple of 4
    payload += t;
    payload_len -= t;

    subpktlen = (subpktlen + 3) & ~3;	// bump to a multiple of 4
    reply_payload += subpktlen;
    reply_payload_space -= subpktlen;
  }

 end_of_subpackets:

  // add the EOP marker
  subpktlen = add_eop(reply_payload, reply_payload_space);
  subpktlen = (subpktlen + 3) & ~3;	// bump to a multiple of 4
  reply_payload += subpktlen;
  reply_payload_space -= subpktlen;

  send_reply(reply, reply_payload - reply);
}


/*
 * Called when an ethernet packet is received.
 * Return true if we handled it here, otherwise
 * it'll be passed on to the DSP Tx pipe
 */
int
eth_pkt_inspector(bsm12_t *sm, int bufno)
{
  u2_eth_packet_t *pkt = (u2_eth_packet_t *) buffer_ram(bufno);
  size_t byte_len = (buffer_pool_status->last_line[bufno] - 3) * 4;

  //static size_t last_len = 0;

  // hal_toggle_leds(0x1);

  // inspect rcvd frame and figure out what do do.

  if (pkt->ehdr.ethertype != U2_ETHERTYPE)
    return true;	// ignore, probably bogus PAUSE frame from MAC

  int chan = u2p_chan(&pkt->fixed);

  switch (chan){
  case CONTROL_CHAN:
    handle_control_chan_frame(pkt, byte_len);
    return -1;
    break;

  case 0:
    return 0;	// pass it off to DSP TX

  case 1:
    return 1;	// pass it off to SERDES TX

  default:
    abort();
    break;
  }
}

/*
 * Called when eth phy state changes (w/ interrupts disabled)
 */
void
link_changed_callback(int speed)
{
  link_is_up = speed != 0;
  hal_set_leds(link_is_up ? LED_RJ45 : 0x0, LED_RJ45);
  printf("\neth link changed: speed = %d\n", speed);
}


void
print_tune_result(char *msg, bool tune_ok,
		  u2_fxpt_freq_t target_freq, struct tune_result *r)
{
#if 0
  printf("db_tune %s %s\n", msg, tune_ok ? "true" : "false");
  putstr("  target_freq   "); print_fxpt_freq(target_freq); newline();
  putstr("  baseband_freq "); print_fxpt_freq(r->baseband_freq); newline();
  putstr("  dxc_freq      "); print_fxpt_freq(r->dxc_freq); newline();
  putstr("  residual_freq "); print_fxpt_freq(r->residual_freq); newline();
  printf("  inverted      %s\n", r->inverted ? "true" : "false");
#endif
}
