/* 
 * USRP - Universal Software Radio Peripheral
 *
 * Copyright (C) 2003 Free Software Foundation, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Boston, MA  02110-1301  USA
 */

#include "usrp_common.h"
#include "delay.h"

void
main (void)
{
  init_usrp ();

  // CPUCS = 0;		// 12 MHz
  // CPUCS = bmCLKSPD0;	// 24 MHz
  CPUCS = bmCLKSPD1;	// 48 MHz
  
  while (1){
    USRP_LED_REG ^= bmLED0;
    mdelay (10);
  }
}
