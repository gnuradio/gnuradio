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
#include "nonstdio.h"
#include "memset_wa.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <clocks.h>
#include <mdelay.h>

// ----------------------------------------------------------------

int packet_number = 0;
volatile bool send_packet_now = 0;

#define SERDES_TX_BUF	0
#define	SERDES_RX_BUF 	1


#define NLINES_PER_PKT	380


// ----------------------------------------------------------------

//static int timer_delta = (int)(MASTER_CLK_RATE * 100e-6);
static int timer_delta = 1000000; // .01 second

void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
  send_packet_now = true;
}


static void
init_packet(int *buf)
{
  int i = 0;
  for (i = 0; i < BP_NLINES; i++){
    buf[i] = ((2*i + 0) << 16) | (2*i+1);
  }
}

static bool
check_packet(int *buf, int nlines)
{
  bool ok = true;
  int i = 0;
  for (i = 0; i < nlines; i++){
    int expected = ((2*i + 0) << 16) | (2*i+1);
    if (buf[i] != expected){
      ok = false;
      printf("buf[%d] = 0x%x  expected = 0x%x\n", i, buf[i], expected);
    }
  }
  return ok;
}

static void
zero_buffer(int bufno)
{
  memset_wa(buffer_ram(bufno), 0, BP_NLINES * 4);
}

static void
init_packets(void)
{
  // init just the one we're using
  init_packet(buffer_ram(SERDES_TX_BUF));
}

int
main(void)
{
  u2_init();

  // We're free running and provide clock to the MIMO interface
  clocks_mimo_config(MC_WE_DONT_LOCK | MC_PROVIDE_CLK_TO_MIMO);


  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  // output_regs->debug_mux_ctrl = 1;
  // hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  // hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  putstr("\nsd_gentest\n");
  
  // Set up serdes (already enabled)
  //output_regs->serdes_ctrl = (SERDES_ENABLE | SERDES_RXEN | SERDES_LOOPEN);
  //output_regs->serdes_ctrl = (SERDES_ENABLE | SERDES_RXEN);

  init_packets();

  // pic_register_handler(IRQ_TIMER, timer_irq_handler);

  //if (hwconfig_simulation_p())
  //  timer_delta = sim_timer_delta;

  // start a receive from sd
  zero_buffer(SERDES_RX_BUF);
  bp_receive_to_buf(SERDES_RX_BUF, PORT_SERDES, 1, 0, BP_LAST_LINE);

  // fire off the first packet
  bp_send_from_buf(SERDES_TX_BUF, PORT_SERDES, 1, 0, NLINES_PER_PKT);
  hal_set_timeout(timer_delta);
  int ready_to_send = 0;

  int counter __attribute__((unused)) = 0;
  int sent = 1;
  int txerr = 0;
  int rxerr = 0;
  int rcvd = 0;
  int rxcrc = 0;
  int sent_acc = 0;
  int txerr_acc = 0;
  int rxerr_acc = 0;
  int rcvd_acc = 0;
  int rxcrc_acc = 0;

#define EXPECTING_PKT() ((counter & 0x1) == 0)
#define	SEND_PKT()      ((counter & 0x1) != 0)

  bool got_packet = false;

  while(1){
    uint32_t status = buffer_pool_status->status;

    if (status & (BPS_DONE(SERDES_RX_BUF))){
      bp_clear_buf(SERDES_RX_BUF);
      got_packet = true;

      //hal_toggle_leds(0x2);

      // check packet
      int last_line = buffer_pool_status->last_line[SERDES_RX_BUF]-1;
      bool ok = check_packet(buffer_ram(SERDES_RX_BUF), last_line);
      
      if (ok) {
	rcvd++;
	//putchar('r');
      }
      else {
	rcvd++;
	rxcrc++;
	//putchar('P');
      }
      // start a receive from sd
      zero_buffer(SERDES_RX_BUF);
      bp_receive_to_buf(SERDES_RX_BUF, PORT_SERDES, 1, 0, BP_LAST_LINE);
    }

    if (status & (BPS_ERROR(SERDES_RX_BUF))){
      bp_clear_buf(SERDES_RX_BUF);
      got_packet = true;
      rcvd++;
      rxerr++;
      //putchar('E');

      // start a receive from sd
      zero_buffer(SERDES_RX_BUF);
      bp_receive_to_buf(SERDES_RX_BUF, PORT_SERDES, 1, 0, BP_LAST_LINE);
    }

    if (status & (BPS_DONE(SERDES_TX_BUF))){
      bp_clear_buf(SERDES_TX_BUF);
      //putchar('t');
      bp_send_from_buf(SERDES_TX_BUF, PORT_SERDES, 1, 0, NLINES_PER_PKT);
      //mdelay(1);
      int	i;
      for (i = 0; i < 50; i++){
	asm volatile ("or  r0, r0, r0\n\
		   or  r0, r0, r0\n    \
		   or  r0, r0, r0\n    \
		   or  r0, r0, r0\n    \
		   or  r0, r0, r0\n    \
		   or  r0, r0, r0\n    \
		   or  r0, r0, r0\n");
      }
      sent ++;
      ready_to_send = 1;
      //hal_toggle_leds(0x1);
    }

    if (status & BPS_ERROR(SERDES_TX_BUF)){
      bp_clear_buf(SERDES_TX_BUF);
      sent++;
      txerr++;
      ready_to_send = 1;
      //putchar('X');
    }

    if(sent >=1000) {
      printf("Status\tSENT %d\tTXERR %d\t",sent,txerr);
      printf("RX %d\tERR %d\tCRC %d\tMISSED %d\n",rcvd, rxerr, rxcrc, sent-rcvd);
      sent_acc += sent; sent = 0;
      txerr_acc += txerr; txerr = 0;
      rcvd_acc += rcvd; rcvd = 0;
      rxerr_acc += rxerr; rxerr = 0;
      rxcrc_acc += rxcrc; rxcrc = 0;
    }

    if(sent_acc >=10000) {
      printf("\nOverall\tSENT %d\tTXERR %d\t",sent_acc,txerr_acc);
      printf("RX %d\tERR %d\tCRC %d\tMISSED %d\n\n",rcvd_acc, rxerr_acc, rxcrc_acc, sent_acc-rcvd_acc);
      sent_acc = 0;
      txerr_acc = 0;
      rcvd_acc = 0;
      rxerr_acc = 0;
      rxcrc_acc = 0;
    }
#if 0
    int pending = pic_regs->pending;
    if (pending & PIC_TIMER_INT){
      hal_set_timeout(timer_delta);
      
      /*
	if (EXPECTING_PKT()){
	if (!got_packet)
	  putchar('T');
	got_packet = false;
      }

      if (SEND_PKT()){
	if (status & BPS_IDLE(SERDES_TX_BUF))
	  bp_send_from_buf(SERDES_TX_BUF, PORT_SERDES, 1, 0, NLINES_PER_PKT);
      }
      counter++;
      */

      putchar('T');
      if(ready_to_send) {
	bp_send_from_buf(SERDES_TX_BUF, PORT_SERDES, 1, 0, NLINES_PER_PKT);
	counter++;
	ready_to_send = 0;
      }
      
      pic_regs->pending = PIC_TIMER_INT;	// clear pending interrupt
    }
#endif
  }
  
  return 0;
}
