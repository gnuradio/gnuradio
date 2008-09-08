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
#include "app_common.h"
#include "print_rmon_regs.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/*
 * Like tx_only.c, but we discard data packets instead of sending them to the
 * DSP TX pipeline.
 */

int total_rx_pkts = 0;
int total_rx_bytes = 0;


static int timer_delta = MASTER_CLK_RATE/1000;	// tick at 1kHz

/*
 * This program can respond to queries from the host
 * and stream rx samples.
 *
 * Buffer 1 is used by the cpu to send frames to the host.
 * Buffers 2 and 3 are used to double-buffer the DSP Rx to eth flow
 * Buffers 4 and 5 are used to double-buffer the eth to DSP Tx  eth flow
 */
//#define CPU_RX_BUF	0	// eth -> cpu
//#define CPU_TX_BUF 	1	// cpu -> eth

#define	DSP_RX_BUF_0	2	// dsp rx -> eth (double buffer)
#define	DSP_RX_BUF_1	3	// dsp rx -> eth
#define	DSP_TX_BUF_0	4	// eth -> dsp tx (double buffer)
#define	DSP_TX_BUF_1	5	// eth -> dsp tx


/*
 * ================================================================
 *      configure DSP TX double buffering state machine
 * ================================================================
 */

// 4 lines of ethernet hdr + 2 lines (word0 + timestamp)
// DSP Tx reads word0 (flags) + timestamp followed by samples

#define DSP_TX_FIRST_LINE		  4
#define DSP_TX_SAMPLES_PER_FRAME	250	// not used except w/ debugging
#define	DSP_TX_EXTRA_LINES		  2	// reads word0 + timestamp

// Receive from ethernet
buf_cmd_args_t dsp_tx_recv_args = {
  PORT_ETH,
  0,
  BP_LAST_LINE
};

// send to DSP Tx
buf_cmd_args_t dsp_tx_send_args = {
  PORT_DSP,
  DSP_TX_FIRST_LINE,	// starts just past ethernet header
  0			// filled in from last_line register
};

dbsm_t dsp_tx_sm;	// the state machine


// ----------------------------------------------------------------


// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
}

// Tx DSP underrun
void
underrun_irq_handler(unsigned irq)
{
  putchar('U');

  dbsm_stop(&dsp_tx_sm);
  dsp_tx_regs->clear_state = 1;
  dbsm_start(&dsp_tx_sm);  // restart sm so we're listening to ethernet again

  // putstr("\nirq: underrun\n");
}


void
start_rx_cmd(const u2_mac_addr_t *host, op_start_rx_t *p)
{
}

void
stop_rx_cmd(void)
{
}

static void
setup_tx()
{
  dsp_tx_regs->clear_state = 1;
  bp_clear_buf(DSP_TX_BUF_0);
  bp_clear_buf(DSP_TX_BUF_1);

  int tx_scale = 256;
  int interp = 32;

  op_config_tx_t def_config;
  memset(&def_config, 0, sizeof(def_config));
  def_config.phase_inc  = 408021893;			// 9.5 MHz [2**32 * fc/fsample]
  def_config.scale_iq = (tx_scale << 16) | tx_scale;
  def_config.interp = interp;

  // setup Tx DSP regs
  config_tx_cmd(&def_config);
}


inline static void
buffer_irq_handler(unsigned irq)
{
  uint32_t  status = buffer_pool_status->status;

  if (status & BPS_ERROR_ALL){
    // FIXME rare path, handle error conditions
    putstr("Errors! status = ");
    puthex32_nl(status);

    printf("total_rx_pkts  = %d\n", total_rx_pkts);
    printf("total_rx_bytes = %d\n", total_rx_bytes);

    print_rmon_regs();

    if (status & (BPS_ERROR(DSP_TX_BUF_0) | BPS_ERROR(DSP_TX_BUF_1))){
      dbsm_stop(&dsp_tx_sm);		
      dsp_tx_regs->clear_state = 1; // try to restart
      dbsm_start(&dsp_tx_sm);
      return;
    }
  }

  dbsm_process_status(&dsp_tx_sm, status);

  if (status & BPS_DONE(CPU_TX_BUF)){
    bp_clear_buf(CPU_TX_BUF);
  }
}


/*
 * Called when an ethernet packet is received.
 *
 * Claim that we handled all the packets,
 * dropping those destined for the TX DSP chain
 * on the ground.
 */
bool
nop_eth_pkt_inspector(dbsm_t *sm, int bufno)
{
  hal_toggle_leds(0x1);

  u2_eth_packet_t *pkt = (u2_eth_packet_t *) buffer_ram(bufno);
  size_t byte_len = (buffer_pool_status->last_line[bufno] - 1) * 4;

  total_rx_pkts++;
  total_rx_bytes += byte_len;

  // inspect rcvd frame and figure out what do do.

  if (pkt->ehdr.ethertype != U2_ETHERTYPE)
    return true;	// ignore, probably bogus PAUSE frame from MAC

  int chan = u2p_chan(&pkt->fixed);

  switch (chan){
  case CONTROL_CHAN:
    handle_control_chan_frame(pkt, byte_len);
    return true;	// we handled the packet
    break;

  case 0:
  default:
    return true;	// We handled the data by dropping it :)
    break;
  }
}


int
main(void)
{
  u2_init();

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  hal_gpio_set_tx_mode(15, 0, GPIOM_FPGA_1);
  hal_gpio_set_rx_mode(15, 0, GPIOM_FPGA_1);

  putstr("\ntx_drop\n");
  
  // Control LEDs
  hal_set_leds(0x0, 0x3);

  pic_register_handler(IRQ_UNDERRUN, underrun_irq_handler);

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

  // initialize double buffering state machine for ethernet -> DSP Tx

  dbsm_init(&dsp_tx_sm, DSP_TX_BUF_0,
	    &dsp_tx_recv_args, &dsp_tx_send_args,
	    nop_eth_pkt_inspector);

  // program tx registers
  setup_tx();

  // kick off the state machine
  dbsm_start(&dsp_tx_sm);

  while(1){
    buffer_irq_handler(0);
  }
}

