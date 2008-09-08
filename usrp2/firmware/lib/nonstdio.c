/* -*- c -*- */
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

#include <nonstdio.h>

static const char hex[16] = "0123456789ABCDEF";

// %x
void
puthex4(unsigned long x)
{
  putchar(hex[x & 0xf]);
}

// %02x
void 
puthex8(unsigned long x)
{
  putchar(hex[(x >> 4) & 0xf]);
  putchar(hex[x & 0xf]);
}

// %04x
void 
puthex16(unsigned long x)
{
  puthex8(x >> 8);
  puthex8(x);
}

// %08x
void 
puthex32(unsigned long x)
{
  puthex16(x >> 16);
  puthex16(x);
}

void 
puthex4_nl(unsigned long x)
{
  puthex4(x);
  newline();
}

void 
puthex8_nl(unsigned long x)
{
  puthex8(x);
  newline();
}

void 
puthex16_nl(unsigned long x)
{
  puthex16(x);
  newline();
}

void 
puthex32_nl(unsigned long x)
{
  puthex32(x);
  newline();
}
