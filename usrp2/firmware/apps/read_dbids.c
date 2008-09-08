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

#include <nonstdio.h>
#include <u2_init.h>
#include <bool.h>
#include <usrp2_i2c_addr.h>
#include <i2c.h>


int main(void)
{
  u2_init();

  puts("\nread_dbids");

  unsigned char dbid_tx[2];
  unsigned char dbid_rx[2];
  bool ok;

  ok = eeprom_read(I2C_ADDR_TX_A, 1, dbid_tx, 2);
  if (!ok){
    puts("failed to read Tx Daugherboard EEPROM");
  }
  else {
    putstr("Tx Daugherboard ID: ");
    puthex8(dbid_tx[1]);    // MSB
    puthex8(dbid_tx[0]);    // LSB
    newline();
  }

  ok = eeprom_read(I2C_ADDR_RX_A, 1, dbid_rx, 2);
  if (!ok){
    puts("failed to read Rx Daugherboard EEPROM");
  }
  else {
    putstr("Rx Daugherboard ID: ");
    puthex8(dbid_rx[1]);    // MSB
    puthex8(dbid_rx[0]);    // LSB
    newline();
  }

  return 0;
}
