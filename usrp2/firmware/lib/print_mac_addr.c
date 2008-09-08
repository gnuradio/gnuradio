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
#include "nonstdio.h"

void
print_mac_addr(const unsigned char addr[6])
{
  puthex8(addr[0]); putchar(':');
  puthex8(addr[1]); putchar(':');
  puthex8(addr[2]); putchar(':');
  puthex8(addr[3]); putchar(':');
  puthex8(addr[4]); putchar(':');
  puthex8(addr[5]);
}

