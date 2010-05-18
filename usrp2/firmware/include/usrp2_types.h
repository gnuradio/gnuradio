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
#ifndef INCLUDED_USRP2_TYPES_H
#define INCLUDED_USRP2_TYPES_H

#include <usrp2_cdefs.h>
#include <stdint.h>

__U2_BEGIN_DECLS

/*!
 * \brief Fixed point representation of a frequency in Hertz (VITA-49 compatible)
 *
 * 64-bit two's complement, with the radix point 20 bits up from the bottom. 
 * Q44.20 format (20 bits to the right of the radix point)
 *
 * Values range from +/- 8.79 terahertz with a resolution of 0.95 microhertz.
 */
typedef int64_t	u2_fxpt_freq_t;

#define U2_FPF_RP	20	// location of radix point in u2_fxpt_freq_t

// macro so we can init structs at compile time
#define U2_DOUBLE_TO_FXPT_FREQ(f) (int64_t)((f) * (1LL << U2_FPF_RP))

static inline u2_fxpt_freq_t
u2_double_to_fxpt_freq(double f)
{
  return U2_DOUBLE_TO_FXPT_FREQ(f);
}

static inline int
u2_fxpt_freq_round_to_int(u2_fxpt_freq_t fx)
{
  return (int)((fx+(1<<(U2_FPF_RP-1)))>>U2_FPF_RP);
}

static inline unsigned int
u2_fxpt_freq_round_to_uint(u2_fxpt_freq_t fx)
{
  return (unsigned int)((fx+(1<<(U2_FPF_RP-1)))>>U2_FPF_RP);
}

static inline double
u2_fxpt_freq_to_double(u2_fxpt_freq_t fx)
{
  return ((double) fx) * 1.0/(1 << U2_FPF_RP);
}

static inline uint32_t
u2_fxpt_freq_hi(u2_fxpt_freq_t f)
{
  return ((f >> 32) & 0xffffffff);
}

static inline uint32_t
u2_fxpt_freq_lo(u2_fxpt_freq_t f)
{
  return (f & 0xffffffff);
}

static inline u2_fxpt_freq_t
u2_fxpt_freq_from_hilo(uint32_t hi, uint32_t lo)
{
  return (((u2_fxpt_freq_t) hi) << 32) | lo;
}

/*!
 * \brief Fixed point representation of a gain in dB (VITA-49 compatible)
 *
 * 16-bit two's complement, with the radix point 7 bits up from the bottom. 
 * Q9.7 format (7 bits to the right of the radix point)
 */
typedef int16_t u2_fxpt_gain_t;

#define U2_FPG_RP	7	// location of radix point in u2_fxpt_gain_t

// macro so we can init structs at compile time
#define U2_DOUBLE_TO_FXPT_GAIN(g) (int16_t)((g) * (1 << U2_FPG_RP))

static inline u2_fxpt_gain_t
u2_double_to_fxpt_gain(double g)
{
  return U2_DOUBLE_TO_FXPT_GAIN(g);
}

static inline float
u2_fxpt_gain_to_double(u2_fxpt_gain_t fx)
{
  return ((double) fx) * 1.0/(1 << U2_FPG_RP);
}

static inline int
u2_fxpt_gain_round_to_int(u2_fxpt_gain_t fx)
{ 
  return (int)((fx+(1<<(U2_FPG_RP-1)))>>U2_FPG_RP);
}


__U2_END_DECLS


#endif /* INCLUDED_USRP2_TYPES_H */
