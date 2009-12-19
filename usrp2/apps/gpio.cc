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
#include <config.h>
#endif

#include <usrp2/usrp2.h>
#include <iostream>
#include <cstdio>

int
main(int argc, char **argv)
{
  usrp2::usrp2::sptr u2 = usrp2::usrp2::make("eth0");

  // Set io_tx[15] as FPGA output
  u2->set_gpio_ddr(usrp2::GPIO_TX_BANK, 0x8000, 0x8000);

  // Set io_tx[15] under host sofware control
  u2->set_gpio_sels(usrp2::GPIO_TX_BANK, "s...............");

  // Write pins
  uint16_t v = 0x8765;
  u2->write_gpio(usrp2::GPIO_TX_BANK, v, 0x8000);

  // Read back
  v = 0;
  u2->read_gpio(usrp2::GPIO_TX_BANK, &v);
  printf("TX GPIO read: %04X\n", v);
  return 0;
}
