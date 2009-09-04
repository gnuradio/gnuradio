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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define	_AL4 __attribute__((aligned (4)))

#define USE_BUFFER_INTERRUPT	0	// 0 or 1


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
#define CPU_TX_BUF 	1	// cpu -> eth

#define	DSP_RX_BUF_0	2	// dsp rx -> eth (double buffer)
#define	DSP_RX_BUF_1	3	// dsp rx -> eth
#define	DSP_TX_BUF_0	4	// eth -> dsp tx (double buffer)
#define	DSP_TX_BUF_1	5	// eth -> dsp tx


/*
 * ================================================================
 *      configure DSP RX double buffering state machine
 * ================================================================
 */


// 4 lines of ethernet hdr + 1 line (word0)
// DSP Rx writes timestamp followed by nlines_per_frame of samples
#define DSP_RX_FIRST_LINE		  5
#define DSP_RX_SAMPLES_PER_FRAME	128
#define	DSP_RX_EXTRA_LINES		  1	// writes timestamp

// Receive from DSP Rx
buf_cmd_args_t dsp_rx_recv_args = {
  PORT_DSP,
  DSP_RX_FIRST_LINE,
  BP_LAST_LINE
};

// send to ethernet
buf_cmd_args_t dsp_rx_send_args = {
  PORT_ETH,
  0,		// starts with ethernet header in line 0
  0,		// filled in from last_line register
};

dbsm_t dsp_rx_sm;	// the state machine

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

/*
 * send constant buffer to DSP TX
 */
static inline void 
SEND_CONST_TO_DSP_TX(void)
{
  bp_send_from_buf(DSP_TX_BUF_0, PORT_DSP, 1,
		   DSP_TX_FIRST_LINE,
		   DSP_TX_FIRST_LINE + DSP_TX_EXTRA_LINES + DSP_TX_SAMPLES_PER_FRAME - 1);
}

// ----------------------------------------------------------------



// The mac address of the host we're sending to.
u2_mac_addr_t host_mac_addr;


void link_changed_callback(int speed);
static volatile bool link_is_up = false;	// eth handler sets this


void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
}

// Tx DSP underrun
void
underrun_irq_handler(unsigned irq)
{
  dsp_tx_regs->clear_state = 1;
  bp_clear_buf(DSP_TX_BUF_0);
  bp_clear_buf(DSP_TX_BUF_1);
  dbsm_stop(&dsp_tx_sm);

  // FIXME anything else?

  putstr("\nirq: underrun\n");
}

// Rx DSP overrun
void
overrun_irq_handler(unsigned irq)
{
  dsp_rx_regs->clear_state = 1;
  bp_clear_buf(DSP_RX_BUF_0);
  bp_clear_buf(DSP_RX_BUF_1);
  dbsm_stop(&dsp_rx_sm);

  // FIXME anything else?

  putstr("\nirq: overrun\n");
}

static void
start_tx_transfers(void)
{
  bp_clear_buf(DSP_TX_BUF_0);	// FIXME, really goes in state machine
  bp_clear_buf(DSP_TX_BUF_1);

  // fill everything with a constant 32k + 0j

  uint32_t const_sample = (32000 << 16) | 0;
  int i;
  for (i = 0; i < BP_NLINES; i++){
    buffer_ram(DSP_TX_BUF_0)[i] = const_sample;
    buffer_ram(DSP_TX_BUF_1)[i] = const_sample;
  }

  /*
   * Construct  ethernet header and word0 and preload into two buffers
   */
  u2_eth_packet_t	pkt;
  memset(&pkt, 0, sizeof(pkt));
  //pkt.ehdr.dst = *host;
  pkt.ehdr.src = *ethernet_mac_addr();
  pkt.ehdr.ethertype = U2_ETHERTYPE;
  u2p_set_word0(&pkt.fixed,
		U2P_TX_IMMEDIATE | U2P_TX_START_OF_BURST, 0);
  u2p_set_timestamp(&pkt.fixed, T_NOW);

  memcpy_wa(buffer_ram(DSP_TX_BUF_0), &pkt, sizeof(pkt));
  memcpy_wa(buffer_ram(DSP_TX_BUF_1), &pkt, sizeof(pkt));


  int tx_scale = 256;

  // setup Tx DSP regs
  dsp_tx_regs->clear_state = 1;			// reset
  dsp_tx_regs->freq = 408021893;		// 9.5 MHz [2**32 * fc/fsample]
  dsp_tx_regs->scale_iq = (tx_scale << 16) | tx_scale;
  dsp_tx_regs->interp_rate = 32;

  // kick off the state machine
  // dbsm_start(&dsp_rx_sm);

  SEND_CONST_TO_DSP_TX();	// send constant buffer to DSP TX
}


void
buffer_irq_handler(unsigned irq)
{
  uint32_t  status = buffer_pool_status->status;

  if (0){
    putstr("irq: ");
    puthex32(status);
    putchar('\n');
  }

  if (status & BPS_ERROR_ALL){
    // FIXME rare path, handle error conditions
  }

  if (status & BPS_DONE(DSP_TX_BUF_0)){
    bp_clear_buf(DSP_TX_BUF_0);
    SEND_CONST_TO_DSP_TX();
    hal_toggle_leds(0x1);
  }

}

int
main(void)
{
  u2_init();

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  //hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  //hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  putstr("\ntx_only\n");
  
  // Control LEDs
  hal_set_leds(0x0, 0x3);

  if (USE_BUFFER_INTERRUPT)
    pic_register_handler(IRQ_BUFFER,   buffer_irq_handler);

  pic_register_handler(IRQ_OVERRUN,  overrun_irq_handler);
  pic_register_handler(IRQ_UNDERRUN, underrun_irq_handler);

  //pic_register_handler(IRQ_TIMER, timer_irq_handler);
  //hal_set_timeout(timer_delta);

  ethernet_register_link_changed_callback(link_changed_callback);

  ethernet_init();

  // initialize double buffering state machine for DSP RX -> Ethernet
  dbsm_init(&dsp_rx_sm, DSP_RX_BUF_0,
	    &dsp_rx_recv_args, &dsp_rx_send_args,
	    dbsm_nop_inspector);

  // setup receive from ETH
  // bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);

#if 0
  if (hwconfig_simulation_p()){
    // If we're simulating, pretend that we got a start command from the host
    u2_mac_addr_t host = {{ 0x00, 0x0A, 0xE4, 0x3E, 0xD2, 0xD5 }};
    start_rx_cmd(&host);
  }
#endif

  start_tx_transfers();		// send constant buffers to DSP TX

  while(1){
    if (!USE_BUFFER_INTERRUPT)
      buffer_irq_handler(0);
  }
}

// ----------------------------------------------------------------

// debugging output on tx pins
#define LS_MASK  0xE0000
#define LS_1000  0x80000
#define LS_100   0x40000
#define LS_10    0x20000

/*
 * Called when eth phy state changes (w/ interrupts disabled)
 */
void
link_changed_callback(int speed)
{
  int v = 0;
  switch(speed){
  case 10:
    v = LS_10;
    link_is_up = true;
    break;
    
  case 100:
    v = LS_100;
    link_is_up = true;
    break;
    
  case 1000:
    v = LS_100;
    link_is_up = true;
    break;

  default:
    v = 0;
    link_is_up = false;
    break;
  }

  //hal_gpio_set_tx(v, LS_MASK);	/* set debug bits on d'board */

  // hal_set_leds(link_is_up ? 0x2 : 0x0, 0x2);

  printf("\neth link changed: speed = %d\n", speed);
}
