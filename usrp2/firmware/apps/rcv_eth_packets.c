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
#include <stddef.h>
#include <stdlib.h>


// ----------------------------------------------------------------

static u2_mac_addr_t dst_mac_addr =
  {{  0xff, 0xff, 0xff, 0xff, 0xff, 0xff }};


// ----------------------------------------------------------------

#define	PACKET_SIZE 1500		// bytes
#define ETH_DATA_RATE 1000000		// 1MB/s
#define	ETH_PACKET_RATE (ETH_DATA_RATE/PACKET_SIZE)	// 13,3333 pkts/s

#define TIMER_RATE 100000000		// 100 MHz clock

static int timer_delta = TIMER_RATE/ETH_PACKET_RATE;	// ticks between interrupts

static volatile bool send_packet_now = false;   // timer handler sets this
static volatile bool link_is_up = false;	// eth handler sets this

int packet_number = 0;

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

  putstr("\neth link changed: speed = ");
  puthex16_nl(speed);
}

void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
  send_packet_now = 1;
}


void
buffer_irq_handler(unsigned irq)
{
  // FIXME
}

static void
init_packet(int *buf, const u2_eth_packet_t *pkt, int bufnum)
{
  int i = 0;
  int mark = ((bufnum & 0xff) << 24) | 0x005A0000;

  for (i = 0; i < BP_NLINES; i++){
    buf[i] = mark | i;
    mark ^= 0x00FF0000;
  }

  // copy header into buffer
  memcpy_wa(buf, pkt, sizeof(*pkt));
}

static void
init_packets(void)
{
  int	i;
  
  u2_eth_packet_t	pkt __attribute__((aligned (4)));

  pkt.ehdr.dst = dst_mac_addr;
  pkt.ehdr.src = *ethernet_mac_addr();
  pkt.ehdr.ethertype = U2_ETHERTYPE;

  // fill ALL buffers for debugging
  for (i = 0; i < 8; i++)
    init_packet((void *)buffer_ram(i), &pkt, i);
}

int
main(void)
{
  u2_init();

  int	prev_leds = -1;
  int	new_leds  = 0x00;
  output_regs->leds = 0x00;

  int peak_hold_count = 0;

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  //hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  //hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  putstr("\nrcv_eth_packets\n");
  
  init_packets();

  // pic_register_handler(IRQ_BUFFER, buffer_irq_handler);  // poll for now

  // FIXME turn off timer since I don't think MTS and MFS instructions are implemented
  // pic_register_handler(IRQ_TIMER, timer_irq_handler);
  // hal_set_timeout(timer_delta);

  ethernet_register_link_changed_callback(link_changed_callback);

  ethernet_init();

  //eth_mac->speed = 4;	// FIXME hardcode mac speed to 1000

  // kick off a receive
  bp_receive_to_buf(2, PORT_ETH, 1, 0, 511);

  while(1){
    // u2_eth_packet_t	pkt;

    new_leds = 0;
    if (link_is_up)
      new_leds = 0x2;

    if ((buffer_pool_status->status & (BPS_DONE_2|BPS_ERROR_2)) != 0){
      // we've got a packet!

#if 0
      // copy to stack buffer so we can byte address it
      memcpy_wa(&pkt, (void *)buffer_ram(2), sizeof(pkt));
      
      putstr("Rx: src: ");
      print_mac_addr(pkt.ehdr.dst_addr);
      putstr(" dst: ");
      print_mac_addr(pkt.ehdr.src_addr);
      putstr(" ethtype: ");
      puthex16(pkt.ehdr.ethertype);
      putstr(" len: ");
      int len = (buffer_pool_status->last_line[2] + 1) * 4;
      puthex16_nl(len);
#else
      volatile int *bp = buffer_ram(2);
      int	i;
      for (i = 0; i < 16; i++){
	puthex8(i);
	putchar(':');
	puthex32_nl(bp[i]);
      }
#endif
      
      // kick off next receive
      bp_clear_buf(2);
      bp_receive_to_buf(2, PORT_ETH, 1, 0, 511);

      peak_hold_count = 2048 * 10;
    }

    if (peak_hold_count > 0){
      peak_hold_count--;
      new_leds |= 0x1;
    }

    if (new_leds != prev_leds){
      prev_leds = new_leds;
      output_regs->leds = new_leds;
    }
  }

  hal_finish();
  return 1;
}
