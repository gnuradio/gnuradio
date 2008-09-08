/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include "bool.h"
#include "ethernet.h"
#include "nonstdio.h"
#include "usrp2_eth_packet.h"
#include "memcpy_wa.h"
#include "dbsm.h"
#include "app_common_v2.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <db.h>
#include <db_base.h>


#define FW_SETS_SEQNO	1	// define to 0 or 1

#if (FW_SETS_SEQNO)
static int fw_seqno;	// used when f/w is filling in sequence numbers
#endif

/*
 * This program can respond to queries from the host
 * and stream rx samples.
 *
 * Buffer 0 is used for rcvd frames from ethernet
 * Buffer 1 is used by the cpu to send frames to the host.
 * Buffers 2 and 3 are used to double-buffer the DSP Rx to eth flow
 */
#define CPU_RX_BUF	0	// eth -> cpu
//#define CPU_TX_BUF 	1	// cpu -> eth
#define	DSP_RX_BUF_0	2	// dsp rx -> eth (double buffer)
#define	DSP_RX_BUF_1	3	// dsp rx -> eth


// variables for streaming mode

static bool     streaming_p = false;
static int      streaming_frame_count = 0;
#define FRAMES_PER_CMD	1000


/*
 * ================================================================
 *      configure DSP RX double buffering state machine
 * ================================================================
 */

// 4 lines of ethernet hdr + 1 line transport hdr + 1 line (word0)
// DSP Rx writes timestamp followed by nlines_per_frame of samples
#define DSP_RX_FIRST_LINE ((sizeof(u2_eth_hdr_t) + sizeof(u2_transport_hdr_t))/4 + 1)

// receive from DSP
buf_cmd_args_t dsp_rx_recv_args = {
  PORT_DSP,
  DSP_RX_FIRST_LINE,
  BP_LAST_LINE
};

// send to ETH
buf_cmd_args_t dsp_rx_send_args = {
  PORT_ETH,
  0,		// starts with ethernet header in line 0
  0,		// filled in from list_line register
};

dbsm_t dsp_rx_sm;	// the state machine

// ----------------------------------------------------------------



// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


void link_changed_callback(int speed);
static volatile bool link_is_up = false;	// eth handler sets this


void
start_rx_streaming_cmd(const u2_mac_addr_t *host, op_start_rx_streaming_t *p)
{
  host_mac_addr = *host;	// remember who we're sending to

  /*
   * Construct  ethernet header and word0 and preload into two buffers
   */
  u2_eth_packet_t	pkt;
  memset(&pkt, 0, sizeof(pkt));
  pkt.ehdr.dst = *host;
  pkt.ehdr.ethertype = U2_ETHERTYPE;
  u2p_set_word0(&pkt.fixed, 0, 0);
  // DSP RX will fill in timestamp

  memcpy_wa(buffer_ram(DSP_RX_BUF_0), &pkt, sizeof(pkt));
  memcpy_wa(buffer_ram(DSP_RX_BUF_1), &pkt, sizeof(pkt));


  if (FW_SETS_SEQNO)
    fw_seqno = 0;

  // setup RX DSP regs
  dsp_rx_regs->clear_state = 1;			// reset

  if (1){			// we're streaming
    streaming_p = true;
    streaming_frame_count = FRAMES_PER_CMD;
    dsp_rx_regs->rx_command =
      MK_RX_CMD(FRAMES_PER_CMD * p->items_per_frame, p->items_per_frame,
		1, 1);			// set "chain" bit

    // kick off the state machine
    dbsm_start(&dsp_rx_sm);
    dsp_rx_regs->rx_time = 0;		// enqueue first of two commands

    // make sure this one and the rest have the "now" and "chain" bits set.
    dsp_rx_regs->rx_command =
      MK_RX_CMD(FRAMES_PER_CMD * p->items_per_frame, p->items_per_frame,
		1, 1);				
    dsp_rx_regs->rx_time = 0;			// enqueue second command
  }
#if 0
  else {
    streaming_p = false;
    dsp_rx_regs->rx_command =
      MK_RX_CMD(p->total_samples, p->items_per_frame, p->rx_now, 0);

    // kick off the state machine
    dbsm_start(&dsp_rx_sm);
    dsp_rx_regs->rx_time = p->rx_time;
  }
#endif
}


void
stop_rx_cmd(void)
{
  streaming_p = false;
  dsp_rx_regs->clear_state = 1;	// flush cmd queue
  bp_clear_buf(DSP_RX_BUF_0);
  bp_clear_buf(DSP_RX_BUF_1);
}

inline static void
buffer_irq_handler(unsigned irq)
{
  uint32_t  status = buffer_pool_status->status;

  if (status & BPS_DONE(CPU_RX_BUF)){	// we've rcvd a frame from ethernet
    bp_clear_buf(CPU_RX_BUF);
    eth_pkt_inspector(0, CPU_RX_BUF);
    bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);
  }
  if (status & BPS_ERROR(CPU_RX_BUF)){	// error from ethernet
    bp_clear_buf(CPU_RX_BUF);
    bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);
  }

  dbsm_process_status(&dsp_rx_sm, status);

  if (status & BPS_DONE(CPU_TX_BUF)){
    bp_clear_buf(CPU_TX_BUF);
  }
}

#if (FW_SETS_SEQNO)
/*
 * Debugging ONLY.  This will be handled by the tx_protocol_engine.
 *
 * This is called when the DSP Rx chain has filled in a packet.
 * We set and increment the seqno, then return false, indicating
 * that we didn't handle the packet.  A bit of a kludge
 * but it should work.
 */
bool 
fw_sets_seqno_inspector(dbsm_t *sm, int buf_this)	// returns false
{
  uint32_t *p = buffer_ram(buf_this);
  uint32_t seqno = fw_seqno++;

  // KLUDGE all kinds of nasty magic numbers and embedded knowledge
  uint32_t t = p[4];
  t = (t & 0xffff00ff) | ((seqno & 0xff) << 8);
  p[4] = t;

  // queue up another rx command when required
  if (streaming_p && --streaming_frame_count == 0){
    streaming_frame_count = FRAMES_PER_CMD;
    dsp_rx_regs->rx_time = 0;
  }

  return false;		// we didn't handle the packet
}
#endif


int
main(void)
{
  u2_init();

  putstr("\nrx_only_v2\n");

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

  // initialize double buffering state machine for DSP RX -> Ethernet

  if (FW_SETS_SEQNO){
    dbsm_init(&dsp_rx_sm, DSP_RX_BUF_0,
	      &dsp_rx_recv_args, &dsp_rx_send_args,
	      fw_sets_seqno_inspector);
  }
  else {
    dbsm_init(&dsp_rx_sm, DSP_RX_BUF_0,
	      &dsp_rx_recv_args, &dsp_rx_send_args,
	      dbsm_nop_inspector);
  }

  // setup receive from ETH
  bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);


  while(1){
    buffer_irq_handler(0);

    int pending = pic_regs->pending;		// poll for under or overrun

    if (pending & PIC_OVERRUN_INT){
      dbsm_handle_rx_overrun(&dsp_rx_sm);
      pic_regs->pending = PIC_OVERRUN_INT;	// clear pending interrupt
      putchar('O');
    }
  }
}
