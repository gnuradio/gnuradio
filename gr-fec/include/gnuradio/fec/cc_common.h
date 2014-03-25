/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_FEC_CC_COMMON_H
#define INCLUDED_FEC_CC_COMMON_H

typedef unsigned char DECISIONTYPE;
typedef unsigned char COMPUTETYPE;

typedef union {
  //decision_t is a BIT vector
  DECISIONTYPE* t;
  unsigned int* w;
  unsigned short* s;
  unsigned char* c;
} decision_t;

typedef union {
  COMPUTETYPE* t;
} metric_t;

struct v {
  COMPUTETYPE *metrics;
  metric_t old_metrics,new_metrics,metrics1,metrics2; /* Pointers to path metrics, swapped on every bit */
  DECISIONTYPE *decisions;
};

#endif /*INCLUDED_FEC_CC_COMMON_H*/
