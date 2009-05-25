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

#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "pic.h"
#include "hal_io.h"
#include "lsadc.h"
#include "lsdac.h"
#include "buffer_pool.h"
#include "hal_uart.h"
#include "i2c.h"
#include "bool.h"
#include "mdelay.h"
#include "ad9777.h"
#include "clocks.h"
#include "db.h"
#include "usrp2_i2c_addr.h"

//#include "nonstdio.h"

unsigned char u2_hw_rev_major;
unsigned char u2_hw_rev_minor;

static inline void
get_hw_rev(void)
{
  bool ok = eeprom_read(I2C_ADDR_MBOARD, MBOARD_REV_LSB, &u2_hw_rev_minor, 1);
  ok &= eeprom_read(I2C_ADDR_MBOARD, MBOARD_REV_MSB, &u2_hw_rev_major, 1);
}

/*
 * We ought to arrange for this to be called before main, but for now,
 * we require that the user's main call u2_init as the first thing...
 */
bool
u2_init(void)
{
  // Set GPIOs to inputs, disable GPIO streaming
  hal_gpio_set_ddr(GPIO_TX_BANK, 0x0000, 0xffff);
  hal_gpio_set_ddr(GPIO_RX_BANK, 0x0000, 0xffff);

  hal_gpio_write(GPIO_TX_BANK, 0x0000, 0xffff);	// init s/w output value to zero
  hal_gpio_write(GPIO_RX_BANK, 0x0000, 0xffff);

  dsp_rx_regs->gpio_stream_enable = 0; // I, Q LSBs come from DSP

  hal_io_init();

  // init spi, so that we can switch over to the high-speed clock
  spi_init();

  // init i2c so we can read our rev
  i2c_init();
  get_hw_rev();

  // set up the default clocks
  clocks_init();

  // clocks_enable_test_clk(true,1);

  // Enable ADCs
  output_regs->adc_ctrl = ADC_CTRL_ON;

  // Set up AD9777 DAC
  ad9777_write_reg(0, R0_1R);
  ad9777_write_reg(1, R1_INTERP_4X | R1_REAL_MIX);
  ad9777_write_reg(2, 0);
  ad9777_write_reg(3, R3_PLL_DIV_1);
  ad9777_write_reg(4, R4_PLL_ON | R4_CP_AUTO);
  ad9777_write_reg(5, R5_I_FINE_GAIN(0));
  ad9777_write_reg(6, R6_I_COARSE_GAIN(0xf));
  ad9777_write_reg(7, 0);	// I dac offset
  ad9777_write_reg(8, 0);
  ad9777_write_reg(9,  R9_Q_FINE_GAIN(0));
  ad9777_write_reg(10, R10_Q_COARSE_GAIN(0xf));
  ad9777_write_reg(11, 0);	// Q dac offset
  ad9777_write_reg(12, 0);
  
  // Initial values for tx and rx mux registers
  dsp_tx_regs->tx_mux = 0x10;
  dsp_rx_regs->rx_mux = 0x44444444;

  // Set up serdes
  output_regs->serdes_ctrl = (SERDES_ENABLE | SERDES_RXEN);

  pic_init();	// progammable interrupt controller
  bp_init();	// buffer pool
  lsadc_init();	    // low-speed ADCs
  lsdac_init();	    // low-speed DACs
  db_init();	    // daughterboard init
  
  hal_enable_ints();

  // flash all leds to let us know board is alive
  hal_set_leds(0x0, 0x1f);
  mdelay(100);
  hal_set_leds(0x1f, 0x1f);
  mdelay(100);
  hal_set_leds(0x1, 0x1f);  // Leave the first one on

#if 0
  // test register readback
  int rr, vv;
  vv = ad9777_read_reg(0);
  printf("ad9777 reg[0] = 0x%x\n", vv);
  
  for (rr = 0x04; rr <= 0x0d; rr++){
    vv = ad9510_read_reg(rr);
    printf("ad9510 reg[0x%x] = 0x%x\n", rr, vv);
  }
#endif
  
  return true;
}
