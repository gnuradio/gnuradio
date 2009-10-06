/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */ 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <usrp/db_flexrf_mimo.h>
#include <fpga_regs_standard.h>
#include <fpga_regs_common.h>
#include <usrp/usrp_prims.h>
#include <usrp_spi_defs.h>


db_flexrf_2400_tx_mimo_a::db_flexrf_2400_tx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_2400_tx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_2400_tx_mimo_a::_refclk_divisor()
{
  return 16;
}

db_flexrf_2400_rx_mimo_a::db_flexrf_2400_rx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_2400_rx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_2400_rx_mimo_a::_refclk_divisor()
{
  return 16;
}
      
db_flexrf_2400_tx_mimo_b::db_flexrf_2400_tx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_2400_tx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_2400_tx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_2400_rx_mimo_b::db_flexrf_2400_rx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_2400_rx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_2400_rx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_1800_tx_mimo_a::db_flexrf_1800_tx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_1800_tx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_1800_tx_mimo_a::_refclk_divisor()
{
  return 16;
}

db_flexrf_1800_rx_mimo_a::db_flexrf_1800_rx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_1800_rx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_1800_rx_mimo_a::_refclk_divisor()
{
  return 16;
}
    
db_flexrf_1800_tx_mimo_b::db_flexrf_1800_tx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_1800_tx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_1800_tx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_1800_rx_mimo_b::db_flexrf_1800_rx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_1800_rx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_1800_rx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_1200_tx_mimo_a::db_flexrf_1200_tx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_1200_tx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_1200_tx_mimo_a::_refclk_divisor()
{
  return 16;
}

db_flexrf_1200_rx_mimo_a::db_flexrf_1200_rx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_1200_rx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_1200_rx_mimo_a::_refclk_divisor()
{
  return 16;
}
    
db_flexrf_1200_tx_mimo_b::db_flexrf_1200_tx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_1200_tx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_1200_tx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_1200_rx_mimo_b::db_flexrf_1200_rx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_1200_rx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_1200_rx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_900_tx_mimo_a::db_flexrf_900_tx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_900_tx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_900_tx_mimo_a::_refclk_divisor()
{
  return 16;
}

db_flexrf_900_rx_mimo_a::db_flexrf_900_rx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_900_rx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_900_rx_mimo_a::_refclk_divisor()
{
  return 16;
}
    
db_flexrf_900_tx_mimo_b::db_flexrf_900_tx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_900_tx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_900_tx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_900_rx_mimo_b::db_flexrf_900_rx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_900_rx(usrp, which)
{
  d_common->R_DIV(16);
}

int db_flexrf_900_rx_mimo_b::_refclk_divisor()
{
  return 1;
}

db_flexrf_400_tx_mimo_a::db_flexrf_400_tx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_400_tx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_400_tx_mimo_a::_refclk_divisor()
{
  return 16;
}
    
db_flexrf_400_rx_mimo_a::db_flexrf_400_rx_mimo_a(usrp_basic_sptr usrp, int which)
  : db_flexrf_400_rx(usrp, which)
{
  _enable_refclk(true);
  d_common->R_DIV(1);
}

int 
db_flexrf_400_rx_mimo_a::_refclk_divisor()
{
  return 16;
}
    
db_flexrf_400_tx_mimo_b::db_flexrf_400_tx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_400_tx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_400_tx_mimo_b::_refclk_divisor()
{
  return 1;
}
    
db_flexrf_400_rx_mimo_b::db_flexrf_400_rx_mimo_b(usrp_basic_sptr usrp, int which)
  : db_flexrf_400_rx(usrp, which)
{
  d_common->R_DIV(16);
}

int 
db_flexrf_400_rx_mimo_b::_refclk_divisor()
{
  return 1;
}
