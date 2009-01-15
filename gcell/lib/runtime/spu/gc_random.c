/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <gcell/spu/gc_random.h>

static int last_val = 0;

# define M  259200	// values from Numerical Recipes in C, 1988
# define A    7141
# define C   54773

void 
gc_set_seed(int seed)
{
  last_val = ((unsigned int) seed) % M;
}

/*
 * Return a uniformly distributed value in the range [0, 1.0)
 * (Linear congruential generator. YMMV. Caveat emptor.)
 */
float
gc_uniform_deviate(void)
{
  last_val = (last_val * A + C) % M;
  return (float) last_val * (1.0f / (float) M);
}
