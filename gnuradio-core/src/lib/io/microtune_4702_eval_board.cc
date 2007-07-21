/* -*- c++ -*- */
/*
 * Copyright 2001,2004 Free Software Foundation, Inc.
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

#include "microtune_4702_eval_board.h"
#include "microtune_eval_board_defs.h"
#include "ppio.h"
#include "microtune_4702.h"

static const int TUNER_I2C_ADDR = 0x60;

microtune_4702_eval_board::microtune_4702_eval_board (int which_pp)
  : microtune_xxxx_eval_board (which_pp)
{
  d_tuner = new microtune_4702 (d_i2c, TUNER_I2C_ADDR);
}

microtune_4702_eval_board::~microtune_4702_eval_board ()
{
  // default is OK
}

static const float RF_MIN_V = 1.0;	// RF AGC control voltages
static const float RF_MAX_V = 4.0;
static const float IF_MIN_V = 2.0;	// IF AGC control voltages
static const float IF_MAX_V = 4.0;

static const float MIN_AGC  =    0;	// bottom of synthetic range
static const float MAX_AGC  = 1000;	// top of synthetic range

static const float CUTOVER_POINT = 667;


// linear is in the range MIN_AGC to MAX_AGC

static float
linear_to_RF_AGC_voltage (float linear)
{
  if (linear >= CUTOVER_POINT)
    return RF_MAX_V;

  float slope = (RF_MAX_V - RF_MIN_V) / CUTOVER_POINT;
  return RF_MIN_V + linear * slope;
}

static float
linear_to_IF_AGC_voltage (float linear)
{
  if (linear < CUTOVER_POINT)
    return IF_MIN_V;

  float slope = (IF_MAX_V - IF_MIN_V) / (MAX_AGC - CUTOVER_POINT);
  return IF_MIN_V + (linear - CUTOVER_POINT) * slope;
}

void
microtune_4702_eval_board::set_AGC (float v)
{
  if (v < MIN_AGC)
    v = MIN_AGC;

  if (v > MAX_AGC)
    v = MAX_AGC;

  float rf_agc_voltage = linear_to_RF_AGC_voltage (v);
  float if_agc_voltage = linear_to_IF_AGC_voltage (v);

  set_RF_AGC_voltage (rf_agc_voltage);
  set_IF_AGC_voltage (if_agc_voltage);
}
