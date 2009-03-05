/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#include "i2c.h"
#include "usrp2_i2c_addr.h"
#include "mdelay.h"
#include "hal_io.h"
#include "nonstdio.h"
#include "bool.h"



int read_dboard_eeprom(int i2c_addr);


#define USRP_DBID_DBS_RX          	 0x0002
#define USRP_DBID_DBS_RX_WITH_CLOCK_MOD  0x000d

const char dbs_rx_rev2_eeprom[] = {
  0xdb, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18
};

#define	LED_VALS   (LED_A | LED_B | LED_C | LED_D)
#define	LED_MASK   (LED_A | LED_B | LED_C | LED_D)

int
main(void)
{
  u2_init();

  puts("\nburn_dbsrx_eeprom\n");

  hal_set_leds(0, ~0);	// all off

  int i2c_addr = I2C_ADDR_RX_A;
  int dbid = read_dboard_eeprom(i2c_addr);
  bool ok;
  const char *msg = 0;

  switch (dbid){
  case -1:
    msg = "No RX daughterboard found";
    goto bad;

  case -2:
    msg = "Invalid RX EEPROM contents";
    goto bad;

  case USRP_DBID_DBS_RX_WITH_CLOCK_MOD:
    msg = "RX Daughterboard already reports being a DBS RX w/ CLOCK_MOD";
    goto good;

  case USRP_DBID_DBS_RX:
    // Says it's a DBS_RX, attempt to burn the EEPROM
    ok = eeprom_write(i2c_addr, 0,
		      dbs_rx_rev2_eeprom, sizeof(dbs_rx_rev2_eeprom));
    if (ok){
      msg = "Successfully programmed db as DBS RX Rev 2.1";
      goto good;
    }
    else {
      msg = "Failed to write daugherboard eeprom";
      goto bad;
    }

  default:
    msg = "Daughterboard is not a DBS RX; ignored";
    goto bad;
  }
  
 good:
  puts(msg);
  hal_set_leds(LED_VALS, LED_MASK);
  while (1)
    ;

 bad:
  puts(msg);
  while(1){
    hal_toggle_leds(LED_VALS);
    mdelay(50);
  }
}
