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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


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

// 4 lines of ethernet hdr + 1 line transport hdr + 2 lines (word0 + timestamp)
// DSP Tx reads word0 (flags) + timestamp followed by samples

#define DSP_TX_FIRST_LINE ((sizeof(u2_eth_hdr_t) + sizeof(u2_transport_hdr_t))/4)

// Receive from ethernet
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


// ----------------------------------------------------------------


// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


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
  //hal_toggle_leds(0x2);

  uint32_t  status = buffer_pool_status->status;

  dbsm_process_status(&dsp_tx_sm, status);

  if (status & BPS_DONE(CPU_TX_BUF)){
    bp_clear_buf(CPU_TX_BUF);
  }
}

int
main(void)
{
  u2_init();

  // Get our clock from the mimo interface

  clocks_enable_test_clk(true,1);
  clocks_mimo_config(MC_WE_LOCK_TO_MIMO);
  
  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  //hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  //hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  putstr("\nserdes_to_dsp\n");

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();


  // initialize double buffering state machine for ethernet -> DSP Tx

  dbsm_init(&dsp_tx_sm, DSP_TX_BUF_0,
	    &dsp_tx_recv_args, &dsp_tx_send_args,
	    eth_pkt_inspector);

  // program tx registers
  setup_tx();

  // kick off the state machine
  dbsm_start(&dsp_tx_sm);

  while(1){
    buffer_irq_handler(0);

    int pending = pic_regs->pending;	// poll for under or overrun

    if (pending & PIC_UNDERRUN_INT){
      dbsm_handle_tx_underrun(&dsp_tx_sm);
      pic_regs->pending = PIC_UNDERRUN_INT;	// clear interrupt
      putchar('U');
    }
  }
}

