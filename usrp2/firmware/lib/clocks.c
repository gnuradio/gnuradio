/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#include <config.h>
#endif
#include <clocks.h>

#include "memory_map.h"
#include "ad9510.h"
#include "spi.h"
#include "u2_init.h"
#include "nonstdio.h"

void 
clocks_init(void)
{
  // Set up basic clocking functions in AD9510
  ad9510_write_reg(0x45, 0x00); // CLK2 drives distribution

  clocks_enable_fpga_clk(true, 1);

  spi_wait();

  // Set up PLL for 10 MHz reference
  // Reg 4, A counter, Don't Care
  ad9510_write_reg(0x05, 0x00);  // Reg 5, B counter MSBs, 0
  ad9510_write_reg(0x06, 0x05);  // Reg 6, B counter LSBs, 5
  // Reg 7, Loss of reference detect, doesn't work yet, 0
  ad9510_write_reg(0x5A, 0x01); // Update Regs


  // FIXME, probably need interface to this...
  timesync_regs->tick_control = 4;

  // Primary clock configuration
  clocks_mimo_config(MC_WE_DONT_LOCK);

  // Set up other clocks
  clocks_enable_test_clk(false, 0);
  clocks_enable_tx_dboard(false, 0);
  clocks_enable_rx_dboard(false, 0);
  clocks_enable_eth_phyclk(false, 0);

  // Enable clock to ADCs and DACs
  clocks_enable_dac_clk(true, 1);
  clocks_enable_adc_clk(true, 1);
}


void
clocks_mimo_config(int flags)
{
  if (flags & _MC_WE_LOCK){
    // Reg 8, Charge pump on, dig lock det, positive PFD, 47
    ad9510_write_reg(0x08, 0x47);
  }
  else {
    // Reg 8, Charge pump off, dig lock det, positive PFD
    ad9510_write_reg(0x08, 0x00);
  }
  
  // Reg 9, Charge pump current, 0x40=3mA, 0x00=650uA
  ad9510_write_reg(0x09, 0x00);
  // Reg A, Prescaler of 2, everything normal 04
  ad9510_write_reg(0x0A, 0x04);
  // Reg B, R Div MSBs, 0
  ad9510_write_reg(0x0B, 0x00);
  // Reg C, R Div LSBs, 1
  ad9510_write_reg(0x0C, 0x01);
  // Reg D, Antibacklash, Digital lock det, 0

  ad9510_write_reg(0x5A, 0x01); // Update Regs

  spi_wait();
  
  // Allow for clock switchover
  
  if (flags & _MC_WE_LOCK){		// WE LOCK
    if (flags & _MC_MIMO_CLK_INPUT) {
      // Turn on ref output and choose the MIMO connector
      output_regs->clk_ctrl = 0x15;  
    }
    else {
      // turn on ref output and choose the SMA
      output_regs->clk_ctrl = 0x1C; 
    }
  }
  else {				// WE DONT LOCK
    // Disable both ext clk inputs
    output_regs->clk_ctrl = 0x10;
  }

  // Do we drive a clock onto the MIMO connector?
  if (flags & MC_PROVIDE_CLK_TO_MIMO)
    clocks_enable_clkexp_out(true,10);
  else
    clocks_enable_clkexp_out(false,0); 
}

bool 
clocks_lock_detect()
{
  if(pic_regs->pending & PIC_CLKSTATUS)
    return true;
  return false;
}

int inline
clocks_gen_div(int divisor)
{
  int L,H;
  L = (divisor>>1)-1;
  H = divisor-L-2;
  return (L<<4)|H;
}

#define CLOCK_OUT_EN 0x08
#define CLOCK_OUT_DIS_CMOS 0x01
#define CLOCK_OUT_DIS_PECL 0x02
#define CLOCK_DIV_DIS 0x80
#define CLOCK_DIV_EN 0x00

#define CLOCK_MODE_PECL 1
#define CLOCK_MODE_LVDS 2
#define CLOCK_MODE_CMOS 3

void 
clocks_enable_XXX_clk(bool enable, int divisor, int reg_en, int reg_div, int mode)
{
  int enable_word, div_word, div_en_word;

  switch(mode) {
  case CLOCK_MODE_PECL :
    enable_word = enable ? 0x08 : 0x0A;
    break;
  case CLOCK_MODE_LVDS :
    enable_word = enable ? 0x02 : 0x03;
    break;
  case CLOCK_MODE_CMOS :
    enable_word = enable ? 0x08 : 0x09;
    break;
  }
  if(enable && (divisor>1)) {
    div_word = clocks_gen_div(divisor);
    div_en_word = CLOCK_DIV_EN;
  }
  else {
    div_word = 0;
    div_en_word = CLOCK_DIV_DIS;
  }

  ad9510_write_reg(reg_en,enable_word); // Output en/dis
  ad9510_write_reg(reg_div,div_word); // Set divisor
  ad9510_write_reg(reg_div+1,div_en_word); // Enable or Bypass Divider
  ad9510_write_reg(0x5A, 0x01);  // Update Regs
}

// Clock 0
void
clocks_enable_test_clk(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x3C,0x48,CLOCK_MODE_PECL);
}

// Clock 1
void
clocks_enable_fpga_clk(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x3D,0x4A,CLOCK_MODE_PECL);
}

// Clock 2 on Rev 3, Clock 5 on Rev 4
void
clocks_enable_clkexp_out(bool enable, int divisor)
{
  if(u2_hw_rev_major == 3)
    clocks_enable_XXX_clk(enable,divisor,0x3E,0x4C,CLOCK_MODE_PECL);
  else if(u2_hw_rev_major == 4) {
    ad9510_write_reg(0x34,0x00);  // Turn on fine delay
    ad9510_write_reg(0x35,0x00);  // Set Full Scale to nearly 10ns
    ad9510_write_reg(0x36,0x1c);  // Set fine delay.  0x20 is midscale
    clocks_enable_XXX_clk(enable,divisor,0x41,0x52,CLOCK_MODE_LVDS);
    
  }
  else
    putstr("ERR: Invalid Rev\n");
}

// Clock 5 on Rev 3, none (was 2) on Rev 4
void
clocks_enable_eth_phyclk(bool enable, int divisor)
{
  if(u2_hw_rev_major == 3)
    clocks_enable_XXX_clk(enable,divisor,0x41,0x52,CLOCK_MODE_LVDS);
  else if(u2_hw_rev_major == 4)
    clocks_enable_XXX_clk(enable,divisor,0x3E,0x4C,CLOCK_MODE_PECL);
  else
    putstr("ERR: Invalid Rev\n");
}

// Clock 3
void
clocks_enable_dac_clk(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x3F,0x4E,CLOCK_MODE_PECL);
}

// Clock 4
void
clocks_enable_adc_clk(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x40,0x50,CLOCK_MODE_LVDS);
}

// Clock 6
void
clocks_enable_tx_dboard(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x42,0x54,CLOCK_MODE_CMOS);
}

// Clock 7
void
clocks_enable_rx_dboard(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x43,0x56,CLOCK_MODE_CMOS);
}
