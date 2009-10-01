/* -*- c++ -*- */
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

#include "eth_mac.h"
#include "memory_map.h"
#include "bool.h"
#include "eth_phy.h"	// for simulation constants
#include "mdelay.h"
#include "stdio.h"

#define PHY_ADDR 1

void
eth_mac_set_addr(const u2_mac_addr_t *src)
{
  eth_mac->ucast_hi = 
    (((unsigned int)src->addr[0])<<8) + 
    ((unsigned int)src->addr[1]);
  eth_mac->ucast_lo = 
    (((unsigned int)src->addr[2])<<24) + 
    (((unsigned int)src->addr[3])<<16) +
    (((unsigned int)src->addr[4])<<8) +
    (((unsigned int)src->addr[5]));
}


void 
eth_mac_init(const u2_mac_addr_t *src)
{
  eth_mac->miimoder = 25;	// divider from CPU clock (50MHz/25 = 2MHz)

  eth_mac_set_addr(src);
  eth_mac->settings = MAC_SET_PAUSE_EN | MAC_SET_PASS_BCAST | MAC_SET_PASS_UCAST | MAC_SET_PAUSE_SEND_EN; 

  eth_mac->pause_time = 38;
  eth_mac->pause_thresh = 1200;

  // set rx flow control high and low water marks
  // unsigned int lwmark = (2*2048 + 64)/4; // 2 * 2048-byte frames + 1 * 64-byte pause frame
  // eth_mac->fc_hwmark = lwmark + 2048/4;  // plus a 2048-byte frame

  //  eth_mac->fc_lwmark = 600;		// there are currently 2047 lines in the fifo
  // eth_mac->fc_hwmark = 1200;
  //eth_mac->fc_padtime = 1700;           // how long before flow control runs out do we 
                                        // request a re-pause.  Units of 8ns (bytes)

  //eth_mac->tx_pause_en = 0;		// pay attn to pause frames sent to us
  //eth_mac->pause_quanta_set = 38;	// a bit more than 1 max frame 16kb/512 + fudge
  //eth_mac->pause_frame_send_en = 0;	// enable sending pause frames
}

int
eth_mac_read_rmon(int addr)
{
  int t = 0;
  /*  
  eth_mac->rmon_rd_addr = addr;
  eth_mac->rmon_rd_apply = 1;
  while(eth_mac->rmon_rd_grant == 0)
    ;

  t = eth_mac->rmon_rd_dout;
  eth_mac->rmon_rd_apply = 0;
  */
  return t;
}

int
eth_mac_miim_read(int addr)
{
  if (hwconfig_simulation_p()){
    switch(addr){
    case PHY_LINK_AN:
      return LANSR_MASTER | LANSR_LINK_GOOD | LANSR_SPEED_1000;
    default:
      return 0;
    }
  }

  int phy_addr = PHY_ADDR;
  eth_mac->miiaddress = ((addr & 0x1f) << 8) | phy_addr;
  eth_mac->miicommand = MIIC_RSTAT;

  while((eth_mac->miistatus & MIIS_BUSY) != 0)
    ;

  int r = eth_mac->miirx_data;
  //printf("MIIM-READ ADDR 0x%x DATA 0x%x\n",addr, r);
  return r;
}

void
eth_mac_miim_write(int addr, int value)
{
  int phy_addr = PHY_ADDR;
  eth_mac->miiaddress = ((addr & 0x1f) << 8) | phy_addr;
  eth_mac->miitx_data = value;
  eth_mac->miicommand = MIIC_WCTRLDATA;

  //printf("MIIM-WRITE ADDR 0x%x VAL 0x%x\n",addr,value);
  while((eth_mac->miistatus & MIIS_BUSY) != 0)
    ;
}

int
eth_mac_miim_read_status(void)
{
  if (hwconfig_simulation_p())
    return 0;

  return eth_mac->miistatus;
}
