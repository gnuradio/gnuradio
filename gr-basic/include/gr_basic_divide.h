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
#ifndef INCLUDED_GR_BASIC_DIVIDE_H
#define INCLUDED_GR_BASIC_DIVIDE_H

#include <gr_basic_api.h>
#include <gr_sync_block.h>

//TODO may support non-homogeneous IO types,
//DIVIDE_S16_S8 (int16 input, int8 output)
//so this will easily fit into the framework.

enum divide_type{
    DIVIDE_FC64,
    DIVIDE_F64,
    DIVIDE_FC32,
    DIVIDE_F32,
    DIVIDE_SC64,
    DIVIDE_S64,
    DIVIDE_SC32,
    DIVIDE_S32,
    DIVIDE_SC16,
    DIVIDE_S16,
    DIVIDE_SC8,
    DIVIDE_S8,
};

class GR_BASIC_API basic_divide : virtual public gr_sync_block{
public:
    typedef boost::shared_ptr<basic_divide> sptr;
};

GR_BASIC_API basic_divide::sptr basic_make_divide(
    divide_type type, const size_t vlen = 1
);

#endif /* INCLUDED_GR_BASIC_DIVIDE_H */
