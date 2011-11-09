/*
 * Copyright 2011 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_BASIC_OP_TYPES_H
#define INCLUDED_GR_BASIC_OP_TYPES_H

//TODO may support non-homogeneous IO types,
//DIVIDE_S16_S8 (int16 input, int8 output)
//so this will easily fit into the framework.

//TODO may support non-homogeneous IO types,
//MULTIPLY_S8_S16 (int8 input, int16 output)
//so this will easily fit into the framework.

//! Most common types for operator blocks (add, multiply, etc...)
enum op_type{
    OP_FC64,
    OP_F64,
    OP_FC32,
    OP_F32,
    OP_SC64,
    OP_S64,
    OP_SC32,
    OP_S32,
    OP_SC16,
    OP_S16,
    OP_SC8,
    OP_S8,
};

#endif /* INCLUDED_GR_BASIC_OP_TYPES_H */
