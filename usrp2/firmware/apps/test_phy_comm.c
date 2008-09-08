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

// check communication with ethernet PHY chip

#include "u2_init.h"
#include "memory_map.h"
#include "hal_io.h"
#include "ethernet.h"
#include "pic.h"
#include "nonstdio.h"


#define DELTA_T     12500000  		// .125s (10ns per tick)
//#define DELTA_T      10000

// debugging output on tx pins
#define LS_MASK  0xE0000
#define LS_1000  0x80000
#define LS_100   0x40000
#define LS_10    0x20000



#define U2_ETHERTYPE 0xBEEF


static volatile int led_link_up_flag = 0;

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
    led_link_up_flag = 0x2;
    break;
    
  case 100:
    v = LS_100;
    led_link_up_flag = 0x2;
    break;
    
  case 1000:
    v = LS_100;
    led_link_up_flag = 0x2;
    break;

  default:
    v = 0;
    led_link_up_flag = 0;
    break;
  }

  //hal_gpio_set_tx(v, LS_MASK);	/* set debug bits on d'board */

  putstr("\neth link changed: speed = ");
  puthex_nl(speed);
}

void
timer_handler(unsigned irq)
{
  static int led_counter = 0;

  hal_set_timeout(DELTA_T);	// schedule next timeout
  output_regs->leds = (led_counter++ & 0x1) | led_link_up_flag;
}

int
main(void)
{
  u2_init();

  putstr("\n test_phy_comm\n");

  pic_register_handler(IRQ_TIMER, timer_handler);
  hal_set_timeout(DELTA_T);	// schedule timeout

  // setup tx gpio bits for GPIOM_FPGA_1 -- fpga debug output
  //hal_gpio_set_sels(GPIO_TX_BANK, "1111111111111111");
  //hal_gpio_set_sels(GPIO_RX_BANK, "1111111111111111");

  ethernet_register_link_changed_callback(link_changed_callback);

  output_regs->phy_ctrl = 1;	/* reset the eth PHY */
  output_regs->phy_ctrl = 0;

  ethernet_init();

  while(1)
    ;

  return 0;
}
