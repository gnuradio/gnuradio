/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#include <stdio.h>

static bool
check (int v, int counter, int offset)
{
  if ((v & 0x0fff) != counter){
    fprintf (stdout, "%08x: expected 0x%04x, got 0x%04x\n", offset, counter, v & 0x0fff);
    return false;
  }

  if (((v >> 12) & 0xf) != (v & 0xf)){
    fprintf (stdout, "%08x: bad high bits 0x%04x\n", offset, v);
    return false;
  }

  return true;
}

int
main (int argc, char **argv)
{
  static const int BUFSIZE = 64 * 1024;
  unsigned short buf[BUFSIZE];

  int	n;
  int	i;
  int	counter = 0;
  int	offset = 0;
  bool  ok = true;

  while ((n = fread (buf, sizeof (short), BUFSIZE, stdin)) != 0){
    for (i = 0; i < n; i++){
      ok &= check (buf[i], counter, offset);
      counter = (counter + 1) & 0x0fff;
      offset++;
    }
  }

  return ok ? 0 : 1;
}
