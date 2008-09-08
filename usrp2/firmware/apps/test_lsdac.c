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

#include <lsdac.h>
#include <u2_init.h>
#include <nonstdio.h>
#include <hal_io.h>

int
main(void)
{
  u2_init();

  puts("\ntest_lsdac");

  unsigned int up_counter = 0;
  unsigned int dn_counter = 0;

  while(1){
    unsigned int v;
    v = up_counter;
    lsdac_write_rx(0, v << 0);
    lsdac_write_rx(1, v << 1);
    lsdac_write_rx(2, v << 2);
    lsdac_write_rx(3, v << 3);

    v = up_counter;
    lsdac_write_tx(0, v << 0);
    lsdac_write_tx(1, v << 1);
    lsdac_write_tx(2, v << 2);
    lsdac_write_tx(3, v << 3);

    up_counter++;
    dn_counter--;
  }
}
