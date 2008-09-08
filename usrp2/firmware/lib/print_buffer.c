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

void
print_buffer(uint32_t *buf, size_t n)
{
  size_t i;
  for (i = 0; i < n; i++){
    if (i % 4 == 0)
      puthex16(i * 4);

    putchar(' ');
    puthex32(buf[i]);
    if (i % 4 == 3)
      newline();
  }

  newline();
}

