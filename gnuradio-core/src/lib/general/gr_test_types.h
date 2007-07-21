/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_TEST_TYPES_H
#define INCLUDED_GR_TEST_TYPES_H

typedef enum {
      CONSUME_NOUTPUT_ITEMS=0,
      CONSUME_NOUTPUT_ITEMS_LIMIT_MAX=1,
      CONSUME_NOUTPUT_ITEMS_LIMIT_MIN=2,
      CONSUME_ALL_AVAILABLE=3,
      CONSUME_ALL_AVAILABLE_LIMIT_MAX=4,
      /*CONSUME_ALL_AVAILABLE_LIMIT_MIN=5,*/
      CONSUME_ZERO=6,
      CONSUME_ONE=7,
      CONSUME_MINUS_ONE=8
  } gr_consume_type_t;

typedef enum {
      PRODUCE_NOUTPUT_ITEMS=0,
      PRODUCE_NOUTPUT_ITEMS_LIMIT_MAX=1,
      /*PRODUCE_NOUTPUT_ITEMS_LIMIT_MIN=2,*/
      PRODUCE_ZERO=6,
      PRODUCE_ONE=7,
      PRODUCE_MINUS_ONE=8
  } gr_produce_type_t;

#endif /* INCLUDED_GR_TEST_TYPES_H */
