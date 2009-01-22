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
#include "app_passthru_v2.h"
#include "memcpy_wa.h"
#include "clocks.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


#define FW_SETS_SEQNO	1	// define to 0 or 1 (FIXME must be 1 for now)

#if (FW_SETS_SEQNO)
static int fw_seqno __attribute__((unused));	// used when f/w is filling in sequence numbers
#endif


/*
 * Full duplex Tx and Rx between ethernet and serdes
 *
 * Buffer 1 is used by the cpu to send frames to the host.
 * Buffers 2 and 3 are used to double-buffer the DSP Rx to eth flow
 * Buffers 4 and 5 are used to double-buffer the eth to DSP Tx  eth flow
 */
//#define CPU_RX_BUF	0	// eth -> cpu

#define	DSP_RX_BUF_0	2	// serdes -> eth (double buffer)
#define	DSP_RX_BUF_1	3	// serdes -> eth
#define	DSP_TX_BUF_0	4	// eth -> serdes (double buffer)
#define	DSP_TX_BUF_1	5	// eth -> serdes

/*
 * ================================================================
 *   configure serdes double buffering state machine (eth -> serdes)
 * ================================================================
 */


// Receive from ethernet
buf_cmd_args_t dsp_tx_recv_args = {
  PORT_ETH,
  0,
  BP_LAST_LINE
};

// send to serdes
buf_cmd_args_t dsp_tx_send_args = {
  PORT_SERDES,
  0,
  0			// filled in from last_line register
};

dbsm_t dsp_tx_sm;	// the state machine

/*
 * ====================================================================
 *   configure serdes RX double buffering state machine (serdes -> eth)
 * ====================================================================
 */

// receive from serdes
buf_cmd_args_t dsp_rx_recv_args = {
  PORT_SERDES,
  0,
  BP_LAST_LINE
};

// send to ETH
buf_cmd_args_t dsp_rx_send_args = {
  PORT_ETH,
  0,		// starts with ethernet header in line 0
  0,		// filled in from list_line register
};

dbsm_t dsp_rx_sm;	// the state machine


// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


// ----------------------------------------------------------------


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
#if 0
  uint32_t *p = buffer_ram(buf_this);
  uint32_t last_line = buffer_pool_status->last_line[buf_this] - sm->last_line_adj;
  printf("fw_sets_seqno_inspector: buf_this = %d, last_line = %d\n",
	 buf_this, last_line);

  print_buffer(p, (last_line + 1));
#endif

#if 0
  uint32_t *p = buffer_ram(buf_this);
  uint32_t seqno = fw_seqno++;

  // KLUDGE all kinds of nasty magic numbers and embedded knowledge
  uint32_t t = p[4];
  t = (t & 0xffff00ff) | ((seqno & 0xff) << 8);
  p[4] = t;
#endif

  return false;		// we didn't handle the packet
}
#endif


inline static void
buffer_irq_handler(unsigned irq)
{
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

  putstr("\neth <-> serdes\n");

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

  // clocks_mimo_config(MC_WE_LOCK_TO_SMA | MC_PROVIDE_CLK_TO_MIMO);
  clocks_mimo_config(MC_WE_DONT_LOCK | MC_PROVIDE_CLK_TO_MIMO);

#if 0
  // make bit 15 of Tx gpio's be a s/w output
  hal_gpio_set_sel(GPIO_TX_BANK, 15, 's');
  hal_gpio_set_ddr(GPIO_TX_BANK, 0x8000, 0x8000);
#endif

#if 1
  output_regs->debug_mux_ctrl = 1;
  hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");
  hal_gpio_set_ddr(GPIO_TX_BANK, 0xffff, 0xffff);
  hal_gpio_set_ddr(GPIO_RX_BANK, 0xffff, 0xffff);
#endif


  // initialize double buffering state machine for ethernet -> serdes

  dbsm_init(&dsp_tx_sm, DSP_TX_BUF_0,
	    &dsp_tx_recv_args, &dsp_tx_send_args,
	    eth_pkt_inspector);


  // initialize double buffering state machine for serdes -> ethernet

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

  // tell app_common that this dbsm could be sending to the ethernet
  ac_could_be_sending_to_eth = &dsp_rx_sm;


  // kick off the state machines
  dbsm_start(&dsp_tx_sm);
  dbsm_start(&dsp_rx_sm);

  //int which = 0;

  while(1){
    // hal_gpio_write(GPIO_TX_BANK, which, 0x8000);
    // which ^= 0x8000;

    buffer_irq_handler(0);
  }
}
