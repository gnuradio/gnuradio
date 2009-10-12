/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#include "app_passthru_v2.h"
#include "buffer_pool.h"
#include "memcpy_wa.h"
#include "ethernet.h"
#include "nonstdio.h"
#include "print_rmon_regs.h"
#include "db.h"
#include "clocks.h"
#include <string.h>

volatile bool link_is_up = false;	// eth handler sets this


// If this is non-zero, this dbsm could be writing to the ethernet
dbsm_t *ac_could_be_sending_to_eth;

//static unsigned char exp_seqno = 0;

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

  // fire it off
  bp_send_from_buf(CPU_TX_BUF, PORT_ETH, 1, 0, reply_len/4);

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
  r->hw_rev = 0x0000;	// FIXME
  // r->fpga_md5sum = ;	// FIXME
  // r->sw_md5sum = ;	// FIXME

  // FIXME Add d'board info, including dbid, min/max gain, min/max freq

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

bool
handle_control_chan_frame(u2_eth_packet_t *pkt, size_t len)
{
  unsigned char reply[sizeof(u2_eth_packet_t) + 4 * sizeof(u2_subpkt_t)] _AL4;
  unsigned char *reply_payload = &reply[sizeof(u2_eth_packet_t)];
  int reply_payload_space = sizeof(reply) - sizeof(u2_eth_packet_t);

  bool handled_it = false;

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

    switch(gp->opcode){
    case OP_EOP:		// end of subpackets
      goto end_of_subpackets;

    case OP_ID:
      subpktlen = op_id_cmd(gp, reply_payload, reply_payload_space);
      handled_it = true;
      break;

    default:
      if (0){
	printf("\npassing on %d\n", gp->opcode);
      }
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

  if (handled_it){
    // add the EOP marker
    subpktlen = add_eop(reply_payload, reply_payload_space);
    subpktlen = (subpktlen + 3) & ~3;	// bump to a multiple of 4
    reply_payload += subpktlen;
    reply_payload_space -= subpktlen;

    send_reply(reply, reply_payload - reply);
  }

  return handled_it;
}


/*
 * Called when an ethernet packet is received.
 * Return true if we handled it here, otherwise
 * it'll be passed on to the DSP Tx pipe
 */
bool
eth_pkt_inspector(dbsm_t *sm, int bufno)
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
    return handle_control_chan_frame(pkt, byte_len);
    break;

  case 0:
  default:
#if 0
    if (last_len != 0){
      if (byte_len != last_len){
	printf("Len: %d last: %d\n", byte_len, last_len);
      }
    }
    last_len = byte_len;

    if((pkt->thdr.seqno) == exp_seqno){
      exp_seqno++;
      //putchar('.');
    }
    else {
      // putchar('S');
      //printf("S%d %d ",exp_seqno,pkt->thdr.seqno);
      exp_seqno = pkt->thdr.seqno + 1;
    }
#endif
    return false;	// pass it on to Tx DSP
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
