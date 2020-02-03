/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_EQUALIZER_H
#define INCLUDED_DIGITAL_EQUALIZER_H

// Equalizer States:
//  IDLE -- just FIR filtering using the current taps
//  TRAINING -- calculating adaptive taps based on training sequence
//  DD -- calculating adaptive taps based on expected constellation points (in algorithm
//  object)

enum class equalizer_state_t { IDLE, TRAINING, DD };

// TODO: Put a common class for LinearEq/DFE here

#endif