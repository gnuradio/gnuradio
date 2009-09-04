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
#include "print_rmon_regs.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


// ----------------------------------------------------------------

static u2_mac_addr_t dst_mac_addr =
  {{  0xff, 0xff, 0xff, 0xff, 0xff, 0xff }};

// ----------------------------------------------------------------

// #define	PACKET_SIZE 1500		// bytes
// #define ETH_DATA_RATE 1000000		// 1MB/s
// #define	ETH_PACKET_RATE (ETH_DATA_RATE/PACKET_SIZE)	// 13,3333 pkts/s

// static int timer_delta = MASTER_CLK_RATE/ETH_PACKET_RATE;	// ticks between interrupts

static int timer_delta = (int)(MASTER_CLK_RATE * 1e-3);		// tick at 1 kHz
static int sim_timer_delta = (int)(MASTER_CLK_RATE * 100e-6);	// tick at 10 kHz

static volatile bool send_packet_now = false;   // timer handler sets this
static volatile bool link_is_up = false;	// eth handler sets this

int packet_number = 0;


#define CPU_TX_BUF	0	// cpu xmits ethernet frames from here
#define CPU_RX_BUF	1	// receive ethernet frames here

// ----------------------------------------------------------------

/*
 * Called when eth phy state changes (w/ interrupts disabled)
 */
void
link_changed_callback(int speed)
{
  link_is_up = speed == 0 ? false : true;
  hal_set_leds(link_is_up ? 0x2 : 0x0, 0x2);
  printf("\neth link changed: speed = %d\n", speed);
}

void
timer_irq_handler(unsigned irq)
{
  hal_set_timeout(timer_delta);	// schedule next timeout
  send_packet_now = 1;
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
  u2_eth_packet_t	pkt __attribute__((aligned (4)));

  memset(&pkt, 0, sizeof(pkt));

  pkt.ehdr.dst = dst_mac_addr;
  pkt.ehdr.src = *ethernet_mac_addr();
  pkt.ehdr.ethertype = U2_ETHERTYPE;
  pkt.fixed.word0 = 0x01234567;
  pkt.fixed.timestamp = 0xffffffff;

  // init just the one we're using
  init_packet((void *)buffer_ram(CPU_TX_BUF), &pkt, CPU_TX_BUF);
}

int
main(void)
{
  int npackets_sent = 0;

  u2_init();

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  //hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  //hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  putstr("\ngen_eth_packets\n");
  
  hal_set_leds(0x0, 0x3);

  init_packets();

  pic_register_handler(IRQ_TIMER, timer_irq_handler);

  if (hwconfig_simulation_p())
    timer_delta = sim_timer_delta;

  hal_set_timeout(timer_delta);

  ethernet_register_link_changed_callback(link_changed_callback);
  ethernet_init();

  /*
  if (hwconfig_simulation_p()){
    eth_mac->speed = 4;	// hardcode mac speed to 1000
    link_is_up = true;
  }
  */

  // fire off a receive from the ethernet
  bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);

  while(1){
    uint32_t status = buffer_pool_status->status;

    if (status & (BPS_DONE(CPU_RX_BUF) | BPS_ERROR(CPU_RX_BUF))){
      bp_clear_buf(CPU_RX_BUF);
      // ignore incoming ethernet packets; they were looped back in sim
      bp_receive_to_buf(CPU_RX_BUF, PORT_ETH, 1, 0, BP_LAST_LINE);
    }

    if (status & (BPS_DONE(CPU_TX_BUF) | BPS_ERROR(CPU_TX_BUF))){
      if (status & BPS_ERROR(CPU_TX_BUF)){
	putchar('E');
      }
      bp_clear_buf(CPU_TX_BUF);
      npackets_sent++;
      if ((npackets_sent & 0xF) == 0){	// print after every 16 packets
	//print_rmon_regs();
	putchar('.');
      }
    }

    if (link_is_up && send_packet_now && (status & BPS_IDLE(CPU_TX_BUF))){
      send_packet_now = false;

      // kick off the next packet
      // FIXME set packet number in packet

      bp_send_from_buf(CPU_TX_BUF, PORT_ETH, 1, 0, 255);	// 1KB total
      hal_toggle_leds(0x1);
    }
  }

  hal_finish();
  return 1;
}
