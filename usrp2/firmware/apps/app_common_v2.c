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

#include "app_common_v2.h"
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
#include <stddef.h>

volatile bool link_is_up = false;	// eth handler sets this
int cpu_tx_buf_dest_port = PORT_ETH;

// If this is non-zero, this dbsm could be writing to the ethernet
dbsm_t *ac_could_be_sending_to_eth;

static unsigned char exp_seqno __attribute__((unused)) = 0;

static bool
set_time(const op_set_time_t *p)
{
  //printf("Setting time: secs %u, ticks %u\n", p->time_secs, p->time_ticks);
  //sr_time64->secs = p->time_secs; //set below...
  sr_time64->ticks = p->time_ticks;
  switch (p->type){
  case OP_SET_TIME_TYPE_NOW:
    sr_time64->imm = 1;
    break;
  case OP_SET_TIME_TYPE_PPS:
    sr_time64->imm = 0;
    break;
  }
  sr_time64->secs = p->time_secs; //set this last to latch the regs
  return true;
}

static inline bool
config_clock_cmd(const op_config_clock_t *p)
{
  //handle the 10 mhz ref source
  clocks_mimo_config(p->flags & MC_REF_CLK_MASK);

  //handle the pps config
  uint32_t pps_flags = 0;
  if (p->flags & MC_PPS_POLARITY_NEG) pps_flags |= 0x00 << 0;
  if (p->flags & MC_PPS_POLARITY_POS) pps_flags |= 0x01 << 0;
  if (p->flags & MC_PPS_SOURCE_SMA)   pps_flags |= 0x00 << 1;
  if (p->flags & MC_PPS_SOURCE_MIMO)  pps_flags |= 0x01 << 1;
  sr_time64->flags = pps_flags;

  return true;
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

  //putstr("peek: addr="); puthex32(p->addr);
  //printf(" bytes=%u\n", p->bytes);

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
  //putstr("poke: addr="); puthex32(p->addr);
  //printf(" bytes=%u\n", bytes);

  uint8_t *src = (uint8_t *)p + sizeof(*p);
  memcpy_wa((void *)p->addr, src, bytes);

  return true;
}

static bool
set_lo_offset_cmd(const op_freq_t *p)
{
  u2_fxpt_freq_t f = u2_fxpt_freq_from_hilo(p->freq_hi, p->freq_lo);
  if (p->opcode == OP_SET_TX_LO_OFFSET)
    return db_set_lo_offset(tx_dboard, f);
  else
    return db_set_lo_offset(rx_dboard, f);
}

static size_t
gpio_read_cmd(const op_gpio_t *p,
	      void *reply_payload, size_t reply_payload_space)
{
  op_gpio_read_reply_t *r = (op_gpio_read_reply_t *) reply_payload;
  if (reply_payload_space < sizeof(*r))	// no room
    return 0;

 // Build reply subpacket

  r->opcode = OP_GPIO_READ_REPLY;
  r->len = sizeof(op_gpio_read_reply_t);
  r->rid = p->rid;
  r->ok = true;
  r->mbz = 0;
  r->value = hal_gpio_read(p->bank);

  return r->len;
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

#define REPLY_PAYLOAD_MAX_LEN (4 * sizeof(u2_subpkt_t))
static eth_mac_addr_t host_mac_addr;

/***********************************************************************
 * Handle input control data and produce output control data
 **********************************************************************/
static void handle_control_packets(
    const void *data_in, size_t len_in,
    void ** data_out, size_t *len_out
){
  //reply memory handled by this function
  static uint8_t buff_out[REPLY_PAYLOAD_MAX_LEN] _AL4;

  // point to the begining of outgoing payload (subpackets)
  uint8_t *reply_payload = buff_out;
  size_t reply_payload_space = sizeof(buff_out);

  // point to beginning of incoming payload (subpackets)
  uint8_t *payload = (uint8_t *)data_in;
  size_t payload_len = len_in;
  
  size_t subpktlen = 0;
  bool ok = false;

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
      subpktlen = config_tx_v2_cmd((op_config_tx_v2_t *) payload, reply_payload, reply_payload_space);
      break;

    case OP_CONFIG_RX_V2:
      subpktlen = config_rx_v2_cmd((op_config_rx_v2_t *) payload, reply_payload, reply_payload_space);
      break;

    case OP_START_RX_STREAMING:
      start_rx_streaming_cmd(&host_mac_addr, (op_start_rx_streaming_t *) payload);
      ok = true;
      goto generic_reply;
    
    case OP_STOP_RX:
      stop_rx_cmd();
      ok = true;
      goto generic_reply;
    
    case OP_BURN_MAC_ADDR:
      ok = ethernet_set_mac_addr(&((op_burn_mac_addr_t *)payload)->addr);
      goto generic_reply;

    case OP_CONFIG_CLOCK:
      ok = config_clock_cmd((op_config_clock_t *) payload);
      goto generic_reply;

    case OP_DBOARD_INFO:
      subpktlen = dboard_info_cmd(gp, reply_payload, reply_payload_space);
      break;

    case OP_SET_TIME:
      ok = set_time((op_set_time_t *) payload);
      goto generic_reply;

    case OP_PEEK:
      subpktlen = peek_cmd((op_peek_t *)payload, reply_payload, reply_payload_space);
      break;

    case OP_POKE:
      ok = poke_cmd((op_poke_t *)payload);
      goto generic_reply;

    case OP_SET_TX_LO_OFFSET:
    case OP_SET_RX_LO_OFFSET:
      ok = set_lo_offset_cmd((op_freq_t *)payload);
      goto generic_reply;

    case OP_RESET_DB:
      db_init();
      ok = true;
      goto generic_reply;

    case OP_GPIO_SET_DDR:
      ok = true;
      hal_gpio_set_ddr(((op_gpio_t *)payload)->bank, 
		       ((op_gpio_t *)payload)->value, 
		       ((op_gpio_t *)payload)->mask);
      goto generic_reply;

    case OP_GPIO_SET_SELS:
      ok = true;
      hal_gpio_set_sels(((op_gpio_set_sels_t *)payload)->bank,
			(char *)(&((op_gpio_set_sels_t *)payload)->sels));
      goto generic_reply;

    case OP_GPIO_READ:
      subpktlen = gpio_read_cmd((op_gpio_t *) payload, reply_payload, reply_payload_space);
      break;

    case OP_GPIO_WRITE:
      ok = true;
      hal_gpio_write(((op_gpio_t *)payload)->bank, 
		     ((op_gpio_t *)payload)->value, 
		     ((op_gpio_t *)payload)->mask);
      goto generic_reply;

    case OP_GPIO_STREAM:
      ok = true;
      dsp_rx_regs->gpio_stream_enable = (uint32_t)((op_gpio_t *)payload)->value;
      goto generic_reply;

    // Add new opcode handlers here

    generic_reply:
      subpktlen = generic_reply(gp, reply_payload, reply_payload_space, ok);
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

  // set the output pointers
  *data_out = buff_out;
  *len_out = sizeof(buff_out) - reply_payload_space;
}

static void
send_reply(void *reply, size_t reply_len)
{
  if (reply_len < 64) reply_len = 64;

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

void
handle_control_chan_frame(u2_eth_packet_pad_before_t *pkt, size_t len)
{
  // process the control data
  host_mac_addr = pkt->ehdr.src;
  void *data_out; size_t len_out;
  handle_control_packets(
    (uint8_t*)pkt + sizeof(u2_eth_packet_pad_before_t),
    len - sizeof(u2_eth_packet_pad_before_t),
    &data_out, &len_out
  );

  // setup reply
  struct {
    uint32_t ctrl_word;
    u2_eth_packet_pad_before_t eth_pkt;
    uint8_t payload[REPLY_PAYLOAD_MAX_LEN];
  } reply _AL4;
  memset(&reply, 0, sizeof(reply));
  size_t total_len = sizeof(reply) - REPLY_PAYLOAD_MAX_LEN + len_out;
  reply.ctrl_word = total_len;
  reply.eth_pkt.ehdr.dst = pkt->ehdr.src;
  reply.eth_pkt.ehdr.src = *ethernet_mac_addr();
  reply.eth_pkt.ehdr.ethertype = U2_CTRL_ETHERTYPE;
  memcpy(reply.payload, data_out, len_out);

  //send the reply
  send_reply(&reply, total_len);
}


/*
 * Called when an ethernet packet is received.
 * Return true if we handled it here, otherwise
 * it'll be passed on to the DSP Tx pipe
 */
bool
eth_pkt_inspector(dbsm_t *sm, int bufno)
{
  u2_eth_packet_pad_before_t *pkt = (u2_eth_packet_pad_before_t *) buffer_ram(bufno);
  size_t byte_len = (buffer_pool_status->last_line[bufno] - 3) * 4;

  // inspect rcvd frame and figure out what do do.
  switch (pkt->ehdr.ethertype){

  case U2_DATA_ETHERTYPE:
    return false;	// pass it on to Tx DSP

  case U2_CTRL_ETHERTYPE:
    handle_control_chan_frame(pkt, byte_len);
    return true;

  }
  return true;	// ignore, probably bogus PAUSE frame from MAC
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
