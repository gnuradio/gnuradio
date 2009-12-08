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
#include "dbsm.h"
#include "app_common_v2.h"
#include "memcpy_wa.h"
#include "clocks.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define FW_SETS_SEQNO	1	// define to 0 or 1 (FIXME must be 1 for now)

#if (FW_SETS_SEQNO)
static int fw_seqno;	// used when f/w is filling in sequence numbers
#endif


/*
 * Full duplex Tx and Rx between serdes and DSP pipelines
 *
 * Buffer 1 is used by the cpu to send frames to the host.
 * Buffers 2 and 3 are used to double-buffer the DSP Rx to serdes flow
 * Buffers 4 and 5 are used to double-buffer the serdes to DSP Tx flow
 */
//#define CPU_RX_BUF	0	// eth -> cpu

#define	DSP_RX_BUF_0	2	// dsp rx -> serdes (double buffer)
#define	DSP_RX_BUF_1	3	// dsp rx -> serdes
#define	DSP_TX_BUF_0	4	// serdes -> dsp tx (double buffer)
#define	DSP_TX_BUF_1	5	// serdes -> dsp tx

/*
 * ==================================================================
 *   configure DSP TX double buffering state machine (serdes -> dsp)
 * ==================================================================
 */

// 4 lines of ethernet hdr + 1 line transport hdr + 2 lines (word0 + timestamp)
// DSP Tx reads word0 (flags) + timestamp followed by samples

#define DSP_TX_FIRST_LINE ((sizeof(u2_eth_hdr_t) + sizeof(u2_transport_hdr_t))/4)

// Receive from serdes
buf_cmd_args_t dsp_tx_recv_args = {
  PORT_SERDES,
  0,
  BP_LAST_LINE
};

// send to DSP Tx
buf_cmd_args_t dsp_tx_send_args = {
  PORT_DSP,
  DSP_TX_FIRST_LINE,	// starts just past transport header
  0			// filled in from last_line register
};

dbsm_t dsp_tx_sm;	// the state machine

/*
 * =================================================================
 *   configure DSP RX double buffering state machine (dsp -> serdes)
 * =================================================================
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

// send to serdes
buf_cmd_args_t dsp_rx_send_args = {
  PORT_SERDES,
  0,		// starts with ethernet header in line 0
  0,		// filled in from list_line register
};

dbsm_t dsp_rx_sm;	// the state machine


// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


// variables for streaming mode

static bool         streaming_p = false;
static unsigned int streaming_items_per_frame = 0;
static int          streaming_frame_count = 0;
#define FRAMES_PER_CMD	1000

bool is_streaming(void){ return streaming_p; }

// ----------------------------------------------------------------


void
restart_streaming(void)
{
  // setup RX DSP regs
  dsp_rx_regs->clear_state = 1;			// reset

  streaming_p = true;
  streaming_frame_count = FRAMES_PER_CMD;

  dsp_rx_regs->rx_command =
    MK_RX_CMD(FRAMES_PER_CMD * streaming_items_per_frame,
	      streaming_items_per_frame,
	      1, 1);			// set "chain" bit

  // kick off the state machine
  dbsm_start(&dsp_rx_sm);

  dsp_rx_regs->rx_time = 0;		// enqueue first of two commands

  // make sure this one and the rest have the "now" and "chain" bits set.
  dsp_rx_regs->rx_command =
    MK_RX_CMD(FRAMES_PER_CMD * streaming_items_per_frame,
	      streaming_items_per_frame,
	      1, 1);

  dsp_rx_regs->rx_time = 0;		// enqueue second command
}

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
  pkt.ehdr.src = *ethernet_mac_addr();
  pkt.ehdr.ethertype = U2_ETHERTYPE;
  u2p_set_word0(&pkt.fixed, 0, 0);
  // DSP RX will fill in timestamp

  memcpy_wa(buffer_ram(DSP_RX_BUF_0), &pkt, sizeof(pkt));
  memcpy_wa(buffer_ram(DSP_RX_BUF_1), &pkt, sizeof(pkt));


  if (FW_SETS_SEQNO)
    fw_seqno = 0;

  streaming_items_per_frame = p->items_per_frame;
  restart_streaming();
}

void start_rx_streaming_at_cmd(const u2_mac_addr_t *host, op_start_rx_streaming_t *p, uint32_t time)
{}
void restart_streaming_at(uint32_t time)
{}

void
stop_rx_cmd(void)
{
  streaming_p = false;
  dsp_rx_regs->clear_state = 1;	// flush cmd queue
  bp_clear_buf(DSP_RX_BUF_0);
  bp_clear_buf(DSP_RX_BUF_1);
}


static void
setup_tx()
{
  dsp_tx_regs->clear_state = 1;
  bp_clear_buf(DSP_TX_BUF_0);
  bp_clear_buf(DSP_TX_BUF_1);

  int tx_scale = 256;
  int interp = 32;

  // setup some defaults

  dsp_tx_regs->freq = 0;
  dsp_tx_regs->scale_iq = (tx_scale << 16) | tx_scale;
  dsp_tx_regs->interp_rate = interp;
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


inline static void
buffer_irq_handler(unsigned irq)
{
  // hal_toggle_leds(LED_A);

  uint32_t  status = buffer_pool_status->status;

  if (0 && (status & ~BPS_IDLE_ALL)){
    putstr("status = ");
    puthex32_nl(status);
  }

  dbsm_process_status(&dsp_tx_sm, status);
  dbsm_process_status(&dsp_rx_sm, status);
}

int
main(void)
{
  u2_init();

  output_regs->led_src = 0x3;		// h/w controls bottom two bits
  clocks_enable_test_clk(true, 1);

  putstr("\nSERDES TxRx\n");

  cpu_tx_buf_dest_port = PORT_SERDES;

  // ethernet_register_link_changed_callback(link_changed_callback);
  // ethernet_init();

  clocks_mimo_config(MC_WE_LOCK_TO_MIMO);

  // puts("post clocks_mimo_config");

#if 0
  // make bit 15 of Tx gpio's be a s/w output
  hal_gpio_set_sel(GPIO_TX_BANK, 15, 's');
  hal_gpio_set_ddr(GPIO_TX_BANK, 0x8000, 0x8000);
#endif

#if 0
  output_regs->debug_mux_ctrl = 1;
  hal_gpio_set_sels(GPIO_TX_BANK, "0000000000000000");
  hal_gpio_set_sels(GPIO_RX_BANK, "0000000000000000");
  hal_gpio_set_ddr(GPIO_TX_BANK, 0xffff, 0xffff);
  hal_gpio_set_ddr(GPIO_RX_BANK, 0xffff, 0xffff);
#endif


  // initialize double buffering state machine for ethernet -> DSP Tx

  dbsm_init(&dsp_tx_sm, DSP_TX_BUF_0,
	    &dsp_tx_recv_args, &dsp_tx_send_args,
	    eth_pkt_inspector);


  //output_regs->flush_icache = 1;

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

  // puts("post dbsm_init's");

  // tell app_common that this dbsm could be sending to the ethernet
  ac_could_be_sending_to_eth = &dsp_rx_sm;


  // program tx registers
  setup_tx();

  // puts("post setup_tx");

  // kick off the state machine
  dbsm_start(&dsp_tx_sm);

  // puts("post dbsm_start");

  //int which = 0;

  while(1){
    // hal_gpio_write(GPIO_TX_BANK, which, 0x8000);
    // which ^= 0x8000;

    buffer_irq_handler(0);

    int pending = pic_regs->pending;		// poll for under or overrun

    if (pending & PIC_UNDERRUN_INT){
      dbsm_handle_tx_underrun(&dsp_tx_sm);
      pic_regs->pending = PIC_UNDERRUN_INT;	// clear interrupt
      putchar('U');
    }

    if (pending & PIC_OVERRUN_INT){
      dbsm_handle_rx_overrun(&dsp_rx_sm);
      pic_regs->pending = PIC_OVERRUN_INT;	// clear pending interrupt

      // FIXME Figure out how to handle this robustly.
      // Any buffers that are emptying should be allowed to drain...

      if (streaming_p){
	// restart_streaming();
	// FIXME report error
      }
      else {
	// FIXME report error
      }
      putchar('O');
    }
  }
}
