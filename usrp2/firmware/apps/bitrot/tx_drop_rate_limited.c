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
#include "eth_mac.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


/*
 * receive packets from ethernet at a fixed rate and discard them
 */

int total_rx_pkts = 0;
int total_rx_bytes = 0;


static int timer_delta = (int)(MASTER_CLK_RATE * 10e-6); // 10us / tick


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



// ----------------------------------------------------------------


// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


static volatile bool receive_packet_now = false;

void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
  receive_packet_now = true;
}


// Tx DSP underrun
void
underrun_irq_handler(unsigned irq)
{
  putchar('U');
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


inline static void
buffer_irq_handler(unsigned irq)
{
  uint32_t  status = buffer_pool_status->status;

  if (status & (BPS_DONE(CPU_TX_BUF) | BPS_ERROR(CPU_TX_BUF)))
    bp_clear_buf(CPU_TX_BUF);

  if (status & (BPS_DONE(DSP_TX_BUF_0) | BPS_ERROR(DSP_TX_BUF_0))){
    bp_clear_buf(DSP_TX_BUF_0);

    if (status & BPS_ERROR(DSP_TX_BUF_0)){
      int crc = eth_mac_read_rmon(0x05);
      int fifo_full = eth_mac_read_rmon(0x06);
      int too_short_too_long = eth_mac_read_rmon(0x07);
      putstr("Errors! status = ");
      puthex32_nl(status);

      printf("crc_err\t\t= %d\n", crc);
      printf("fifo_full\t\t= %d\n", fifo_full);
      printf("too_short_too_long\t= %d\n", too_short_too_long);

      printf("total_rx_pkts  = %d\n", total_rx_pkts);
      printf("total_rx_bytes = %d\n", total_rx_bytes);
    }
    else
      nop_eth_pkt_inspector(0, DSP_TX_BUF_0);
  }

  if (receive_packet_now && (status & BPS_IDLE(DSP_TX_BUF_0))){
    receive_packet_now = false;
    bp_receive_to_buf(DSP_TX_BUF_0, PORT_ETH, 1, 0, BP_LAST_LINE);
  }
}


int
main(void)
{
  u2_init();

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  hal_gpio_set_tx_mode(15, 0, GPIOM_FPGA_1);
  hal_gpio_set_rx_mode(15, 0, GPIOM_FPGA_1);

  putstr("\ntx_drop_rate_limited\n");
  
  // Control LEDs
  hal_set_leds(0x0, 0x3);

  pic_register_handler(IRQ_UNDERRUN, underrun_irq_handler);

  pic_register_handler(IRQ_TIMER, timer_irq_handler);
  hal_set_timeout(timer_delta);

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

  // program tx registers
  setup_tx();

  // start a receive from ethernet
  bp_receive_to_buf(DSP_TX_BUF_0, PORT_ETH, 1, 0, BP_LAST_LINE);

  while(1){
    buffer_irq_handler(0);
  }
}

