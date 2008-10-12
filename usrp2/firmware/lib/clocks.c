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

void 
clocks_init(void)
{
  // Set up basic clocking functions in AD9510
  ad9510_write_reg(0x45, 0x00); // CLK2 drives distribution
  ad9510_write_reg(0x3D, 0x00); // Turn on output 1 (FPGA CLK), normal levels
  ad9510_write_reg(0x4B, 0x80); // Bypass divider 1
  ad9510_write_reg(0x5A, 0x01); // Update Regs

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

  // ETH phy clock
  ad9510_write_reg(0x41, 0x01); // Turn off output 5 (phy_clk)
  ad9510_write_reg(0x53, 0x80); // Bypass divider

  // Enable clock to ADCs and DACs
  ad9510_write_reg(0x3F, 0x00); // Turn on output 3 (DAC CLK), normal levels
  ad9510_write_reg(0x4F, 0x80); // Bypass Div #3

  ad9510_write_reg(0x40, 0x02); // Turn on out 4 (ADC clk), LVDS
  ad9510_write_reg(0x51, 0x80); // Bypass Div #4

  ad9510_write_reg(0x5A, 0x01); // Update Regs
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

  if (flags & MC_PROVIDE_CLK_TO_MIMO) {
    ad9510_write_reg(0x3E, 0x00); // Turn on output 2 (clk_exp_out), normal levels
    ad9510_write_reg(0x4D, 0x00); // Turn on Div2
    ad9510_write_reg(0x4C, 0x44); // Set Div2 = 10, output a 10 MHz clock
  }
  else {
    ad9510_write_reg(0x3E, 0x02); // Turn off output 2 (clk_exp_out)
    ad9510_write_reg(0x4D, 0x80); // Bypass divider 2
  }
  ad9510_write_reg(0x5A, 0x01); // Update Regs
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

void 
clocks_enable_XXX_clk(bool enable, int divisor, int reg_en, int reg_div, int val_off)
{
  if(enable) {
    ad9510_write_reg(reg_en,CLOCK_OUT_EN);     // Turn on output, normal levels
    if(divisor>1) {
      ad9510_write_reg(reg_div,clocks_gen_div(divisor)); // Set divisor
      ad9510_write_reg(reg_div+1,CLOCK_DIV_EN);   // Enable divider
    }
    else {
      ad9510_write_reg(reg_div+1,CLOCK_DIV_DIS);  // Disable Divider
    }
  }
  else {
    ad9510_write_reg(reg_en,val_off);  // Power off output (val different for PECL/CMOS)
    ad9510_write_reg(reg_div+1,CLOCK_DIV_DIS);  // Bypass Divider to power it down
  }
  ad9510_write_reg(0x5A, 0x01);  // Update Regs
}

void
clocks_enable_test_clk(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x3C,0x48,CLOCK_OUT_DIS_PECL);
}

void
clocks_enable_rx_dboard(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x43,0x56,CLOCK_OUT_DIS_CMOS);
}

void
clocks_enable_tx_dboard(bool enable, int divisor)
{
  clocks_enable_XXX_clk(enable,divisor,0x42,0x54,CLOCK_OUT_DIS_CMOS);
}
