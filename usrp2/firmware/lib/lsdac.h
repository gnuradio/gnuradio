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
#ifndef INCLUDED_LSDAC_H
#define INCLUDED_LSDAC_H

#include "memory_map.h"

/*!
 * \brief One time call to initialize low-speed DACs.
 */
void lsdac_init(void);

/*!
 * \brief Write one of the low-speed Rx daughterboard DACs.
 * \param which_dac in [0, 3]
 * \param unsigned 12-bit value in [0, 4095]
 *
 * value maps linearly to output voltage from 0 to 3.3V
 */
void lsdac_write_rx(int which_dac, int value);

/*!
 * \brief Write one of the low-speed Tx daughterboard DACs.
 * \param which_dac in [0, 3]
 * \param unsigned 12-bit value in [0, 4095]
 *
 * value maps linearly to output voltage from 0 to 3.3V
 */
void lsdac_write_tx(int which_dac, int value);


#endif /* INCLUDED_LSDAC_H */
