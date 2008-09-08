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

/*
 * print uint64_t
 */
void
print_uint64(uint64_t u)
{
  const char *_hex = "0123456789ABCDEF";
  if (u >= 10)
    print_uint64(u/10);
  putchar(_hex[u%10]);
}

static void
print_thousandths(int thousandths)
{
  putchar('.');
  if (thousandths < 100)
    putchar('0');
  if (thousandths < 10)
    putchar('0');
  printf("%d", thousandths);
}


void 
print_fxpt_freq(u2_fxpt_freq_t v)
{
  if (v < 0){
    v = -v;
    putchar('-');
  }

  int64_t int_part = v >> 20;
  int32_t frac_part = v & ((1 << 20) - 1);
  
#if 0  
  // would work, if we had it
  printf("%lld.%03d", int_part, (frac_part * 1000) >> 20);
#else
  print_uint64(int_part);
  print_thousandths((frac_part * 1000) >> 20);
#endif
}

void
print_fxpt_gain(u2_fxpt_gain_t v)
{
  if (v < 0){
    v = -v;
    putchar('-');
  }

  int32_t int_part = v >> 7;
  int32_t frac_part = v & ((1 << 7) - 1);

#if 0  
  // would work, if we had it
  printf("%d.%03d", int_part, (frac_part * 1000) >> 7);
#else
  printf("%d", int_part);
  print_thousandths((frac_part * 1000) >> 7);
#endif
}

