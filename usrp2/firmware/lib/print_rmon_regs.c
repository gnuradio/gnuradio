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
#include "config.h"
#endif

#include "print_rmon_regs.h"
#include "eth_mac.h"
#include "nonstdio.h"

void
print_rmon_regs(void)
{
  int i;
  for (i=0; i <= 0x10; i++){
    putstr("RMON[0x");
    puthex8(i);
    putstr("] = ");
    printf("%d\n", eth_mac_read_rmon(i));
  }

  for (i=0x20; i <= 0x30; i++){
    putstr("RMON[0x");
    puthex8(i);
    putstr("] = ");
    printf("%d\n", eth_mac_read_rmon(i));
  }
}
