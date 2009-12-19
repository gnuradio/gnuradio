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

#include <usrp2/strtod_si.h>
#include <stdlib.h>

#define true  1
#define false 0

int
strtod_si(const char *s, double *result)
{
  *result = 0;

  char *endptr;
  double r = strtod(s, &endptr);
  if (s == endptr)
    return false;

  switch (*endptr){
  case 'p': r *= 1e-12; break;
  case 'n': r *= 1e-9;  break;
  case 'u': r *= 1e-6;  break;
  case 'm': r *= 1e-3;  break;
  case 'k': r *= 1e3;   break;
  case 'M': r *= 1e6;   break;
  case 'G': r *= 1e9;   break;
  case 'T': r *= 1e12;  break;
  default:
    // ignore. FIXME could be more robust
    break;
  }

  *result = r;
  return true;
}


